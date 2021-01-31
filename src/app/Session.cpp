#include "paddock/defines.h"

#include "Session.hpp"

#include "Program.hpp"

#include "pads/pads.hpp"

#include "utils.hpp"

#include "midi/Engine.hpp"
#include "midi/errors.hpp"
#include "midi/pads/KorgPadKontrol.hpp"

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

    bool hasSession() const { return _nsmSession.has_value(); }

    const std::string& name() const { return _name; }

    std::error_code open(const std::string& projectPath,
                         const std::string& sessionName,
                         const std::string& clientId)
    {
        if (auto ret = execInMainThread([&] {
                openProgram(projectPath + "/program.pad");
                _name = clientId;
            });
            !ret)
        {
            return ret.error();
        }
        return std::error_code{}; // success
    }

    std::error_code save()
    {
        core::log() << "SAVE";
        return std::error_code{}; // success
    }

    void sendDirty(bool dirty)
    {
        if (_nsmSession)
            _nsmSession->setDirty(dirty);
    }

    std::error_code initMidi()
    {
        auto engine = midi::Engine::create();
        if (!engine)
            return engine.error();

        _midiEngine = std::move(*engine);
        _midiEngine->setEngineEventCallback(
            [this](const midi::events::EngineEvent& event) {
                execInMainThreadAsync(
                    [this, event] { _processEngineEvent(event); });
            });

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

    std::error_code openProgram(std::string filePath)
    {
        // TODO load a program
        auto program = new Program(_parent);

        if (!program)
            return std::make_error_code(std::errc::io_error);

        _filePath = std::move(filePath);

        if (_program)
            _program->deleteLater();

        _program = program;

        connect(_program, &Program::dirtyChanged,
                [this]() { this->sendDirty(_program->dirty()); });

        emit _parent->programChanged();
        return std::error_code{}; // success
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
}; // namespace paddock

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
    return _impl->hasSession();
}

const std::string& Session::name() const
{
    return _impl->name();
}

QVariant Session::controller()
{
    return _impl->controller();
}

std::error_code Session::openProgram(const std::string& filePath)
{
    return _impl->openProgram(filePath);
}

} // namespace paddock
