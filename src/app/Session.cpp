#include "paddock/defines.h"

#include "Session.hpp"

#include "Program.hpp"

#include "pads/korgPadKontrol/Program.hpp"
#include "pads/pads.hpp"

#include "utils.hpp"

#include "midi/Engine.hpp"
#include "midi/errors.hpp"
#include "midi/pads/KorgPadKontrol.hpp"

#include "io/Session.hpp"

#include "core/Log.hpp"

#include "utils/overloaded.hpp"

#ifdef PADDOCK_USE_LIBLO
#include "core/NsmSession.hpp"
#endif

#include <QTimer>

namespace paddock
{
#ifndef PADDOCK_USE_LIBLO
namespace core
{
struct NsmSession
{
    struct Callbacks
    {
        template <typename... Ts>
        Callbacks(Ts...)
        {
        }
    };

    void setDirty(bool) {}
};
} // namespace core

std::optional<core::NsmSession> tryStartNsmSession(core::NsmSession::Callbacks)
{
    return std::nullopt;
}
#else
std::optional<core::NsmSession> tryStartNsmSession(
    core::NsmSession::Callbacks callbacks)
{
    auto nsmUrl = getenv("NSM_URL");
    if (nsmUrl == nullptr)
        return std::nullopt;

    auto session =
        core::NsmSession::startNsmSession(nsmUrl, std::move(callbacks));
    if (!session)
    {
        // TODO print error
        return std::nullopt;
    }
    return std::move(*session);
}
#endif

class Session::_Impl
{
public:
    _Impl(Session* parent)
        : _parent(parent)
    {
        // Don't rely on default intialization, otherwise if this member
        // variable is not declared last the open notification could be
        // accessing a half contructed object.
        _nsmSession = tryStartNsmSession(core::NsmSession::Callbacks{
            [](bool) {},
            [this](const auto&... args) { return this->open(args...); },
            [this]() { return this->save(); }});
    }

    ~_Impl()
    {
        // We need to ensure the controller is destroyed before the engine.
        if (_padController)
            std::visit([](auto controller) { delete controller; },
                       *_padController);
    }

    bool hasNsmSession() const { return _nsmSession.has_value(); }

    const std::string& name() const { return _name; }

    std::error_code open(const std::string& projectPath,
                         const std::string& /* sessionName */,
                         const std::string& clientId)
    {
        if (auto ret = execInMainThread([&] {
                auto ret = openSession(projectPath);
                if (ret)
                    _name = clientId;
                return ret;
            });
            !ret)
        {
            return ret.error();
        }
        return std::error_code{}; // success
    }

    std::error_code save()
    {
        if (auto ret = execInMainThread([&] { return saveSession(_filePath); });
            !ret)
        {
            return ret.error();
        }
        return std::error_code{}; // success
    }

    void sendDirty(bool dirty)
    {
        if (_nsmSession)
            _nsmSession->setDirty(dirty);
    }

    std::error_code initMidi()
    {
        if (_midiEngine)
            return std::error_code{};

        auto engine = midi::Engine::create();
        if (!engine)
            return engine.error();

        _midiEngine = std::move(*engine);
        _midiEngine->setEngineEventCallback(
            [this](const midi::events::EngineEvent& event) {
                execInMainThreadAsync(
                    [this, event] { _processEngineEvent(event); });
            });

        if (!hasNsmSession())
            return lookUpForKnownController();
        return std::error_code{};
    }

    std::error_code lookUpForKnownController()
    {
        auto controller = makePad(_parent, &*_midiEngine, name());

        if (controller)
        {
            _padController = std::make_optional(std::move(*controller));
        }
        else
        {
            if (controller.error() != midi::EngineError::noDeviceFound)
            {
                return controller.error();
            }
            _padController = std::nullopt;
        }

        emit _parent->controllerChanged();

        _updateProgram();

        return std::error_code{};
    }

    QVariant controller()
    {
        if (!_padController)
            return {};
        return std::visit(
            [](auto controller) { return QVariant::fromValue(controller); },
            *_padController);
    }

    std::error_code openSession(std::string filePath)
    {
        initMidi();

        _filePath = std::move(filePath);

        core::log() << "Opening session " << filePath;

        auto session = io::readSession(_filePath);

        if (!session)
        {
            core::log() << session.error().message();
            return lookUpForKnownController();
        }

        _padController = makePad(_parent, session->model);
        assert(_padController);

        tryReconnectPad(*_padController, &*_midiEngine, name());

        emit _parent->controllerChanged();

        if (auto ret = std::visit(
                [&session](const auto& controller) {
                    return controller->program()->deserialize(session->program);
                },
                *_padController);
            !ret)
        {
            core::log() << ret.message();
            return lookUpForKnownController();
        }

        _updateProgram();

        return std::error_code{}; // success
    }

    std::error_code saveSession(std::string filePath) const
    {
        if (_padController)
            return midi::EngineError::noDeviceFound;

        return std::visit(
            [&filePath](const auto& controller) {
                return io::writeSession(
                    filePath,
                    {controller->model, controller->program()->serialize()});
            },
            *_padController);
    }

    Program* program() const { return _program; }

private:
    Session* _parent;

    std::optional<midi::Engine> _midiEngine;
    std::optional<Pad> _padController;

    Program* _program{nullptr};
    std::string _filePath;
    std::optional<core::NsmSession> _nsmSession;
    std::string _name{"Paddock"};

    void _processEngineEvent(const midi::events::EngineEvent& event)
    {
        using namespace midi::events;
        std::visit(overloaded{[this](const ClientStart& event) {
                                  _processClientStartEvent(event);
                              },
                              [this](const ClientExit& event) {
                                  _processClientExitEvent(event);
                              },
                              [this](auto&&) {}},
                   event);
    }

    void _processClientStartEvent(const midi::events::ClientStart& event)
    {
        const auto retryOrLogError =
            [this](const std::error_code error,
                   const midi::events::ClientStart& event) {
                if (error == midi::EngineError::deviceNotReady)
                {
                    // The HW dev files associated with the input ports may not
                    // be ready by the time the system announces the MIDI clien.
                    // In that case, we need to try connecting again a bit
                    // later.

                    QTimer::singleShot(200, [this, event]() {
                        _processClientStartEvent(event);
                    });
                }
                else if (error != midi::EngineError::clientIsNotADevice)
                {
                    core::log() << error.message();
                }
            };

        if (_padController)
        {
            if (std::visit(
                    [](auto&& controller) { return controller->isConnected(); },
                    *_padController))
            {
                return;
            }

            if (auto error = tryReconnectPad(*_padController, &*_midiEngine,
                                             name(), event.client))
            {
                retryOrLogError(error, event);
                return;
            }
            return;
        }

        auto controller = makePad(_parent, &*_midiEngine, name(), event.client);
        if (controller)
        {
            _padController = std::move(*controller);
            emit _parent->controllerChanged();
        }
        else
        {
            retryOrLogError(controller.error(), event);
        }
    }

    void _processClientExitEvent(const midi::events::ClientExit& event)
    {
        if (!_padController)
            return;
        std::visit(
            [&event](auto&& controller) {
                if (controller->deviceId() == event.client)
                    controller->disconnect();
            },
            *_padController);
    }

    void _updateProgram()
    {
        _program =
            std::visit([](auto controller) { return controller->program(); },
                       *_padController);

        connect(_program, &Program::dirtyChanged,
                [this]() { this->sendDirty(_program->dirty()); });

        emit _parent->programChanged();
    }
};

Session::Session()
    : _impl{std::make_unique<_Impl>(this)}
{
}

Session::~Session() = default;

std::error_code Session::init()
{
    return _impl->initMidi();
}

Program* Session::program() const
{
    return _impl->program();
}

bool Session::isNsmSession() const
{
    return _impl->hasNsmSession();
}

const std::string& Session::name() const
{
    return _impl->name();
}

QVariant Session::controller()
{
    return _impl->controller();
}

std::error_code Session::open(const std::string& filePath)
{
    return _impl->openSession(filePath);
}

std::error_code Session::save(const std::string& filePath) const
{
    return _impl->saveSession(filePath);
}

void Session::save(const QUrl& url) const
{
    save(url.toLocalFile().toStdString());
}

} // namespace paddock
