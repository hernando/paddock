#include "KorgPadKontrol.hpp"

#include "korgPadKontrol/Program.hpp"

#include "core/Log.hpp"

#include "midi/errors.hpp"
#include "midi/pads/KorgPadKontrol.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QObject>

namespace paddock
{
class KorgPadKontrol::_Impl
{
public:
    _Impl(KorgPadKontrol* parent)
        : _parent{parent}
        , _program{nullptr}
    {
    }

    _Impl(KorgPadKontrol* parent, midi::KorgPadKontrol&& controller)
        : _parent{parent}
        , _controller{std::move(controller)}
        , _program{nullptr}
    {
        setProgram(new korgPadKontrol::Program{parent});
    }

    bool isConnected() const { return static_cast<bool>(_controller); }

    bool isNative() const
    {
        return _controller &&
               _controller->mode() == midi::KorgPadKontrol::Mode::native;
    }

    std::error_code setMode(midi::KorgPadKontrol::Mode mode)
    {
        if (!_controller)
            return midi::EngineError::noDeviceFound;

        return _controller->setMode(mode);
    }

    void disconnect() { _controller = std::nullopt; }

    std::error_code setController(midi::KorgPadKontrol&& controller)
    {
        _controller = std::move(controller);
        assert(_program);

        return _getOrSetScene();
    }

    midi::ClientId deviceId() const
    {
        if (!_controller)
            return midi::ClientId{};
        return _controller->deviceId();
    }

    korgPadKontrol::Program* program() { return _program; }

    void setProgram(korgPadKontrol::Program* prog)
    {
        if (_program)
            _program->disconnect(_parent);

        _program = prog;
        if (_program)
        {
            _getOrSetScene();
        }
    }

    std::error_code loadDeviceSceneIntoProgram()
    {
        if (!_program)
            return std::error_code{};

        const auto scene = _queryCurrentScene();
        if (!scene)
            return scene.error();

        _program->resetScene(*scene);

        return std::error_code{};
    }

    std::error_code uploadProgramToDevice()
    {
        if (!_controller)
            return midi::EngineError::noDeviceFound;

        if (!_program->hasScene())
            return midi::ProgramError::invalidProgram;

        return _controller->setProgram(_program->midiProgram());
    }

private:
    KorgPadKontrol* _parent;
    std::optional<midi::KorgPadKontrol> _controller;

    korgPadKontrol::Program* _program;

    Expected<midi::korgPadKontrol::Scene> _queryCurrentScene()
    {
        if (!_controller)
            return tl::unexpected(midi::EngineError::noDeviceFound);
        return _controller->queryCurrentScene();
    }

    std::error_code _getOrSetScene()
    {
        if (!_controller || !_program)
            return std::error_code{};

        if (_program->hasScene())
        {
            _controller->setProgram(_program->midiProgram());
        }
        else
        {
            auto currentScene = _controller->queryCurrentScene();
            if (!currentScene)
                return currentScene.error();
            _program->resetScene(std::move(*currentScene));
        }
        return std::error_code{};
    }
};

KorgPadKontrol::KorgPadKontrol(QObject* parent)
    : QObject(parent)
    , _impl(std::make_unique<_Impl>(this))
{
}

KorgPadKontrol::KorgPadKontrol(QObject* parent,
                               midi::KorgPadKontrol&& controller)
    : QObject(parent)
    , _impl(std::make_unique<_Impl>(this, std::move(controller)))
{
}

KorgPadKontrol::~KorgPadKontrol() = default;

bool KorgPadKontrol::isNative() const
{
    return _impl->isNative();
}

bool KorgPadKontrol::isConnected() const
{
    return _impl->isConnected();
}

void KorgPadKontrol::disconnect()
{
    _impl->disconnect();
    emit isConnectedChanged();
}

std::error_code KorgPadKontrol::setController(midi::KorgPadKontrol&& controller)
{
    if (auto error = _impl->setController(std::move(controller)))
        return error;
    emit isConnectedChanged();

    return std::error_code{};
}

midi::ClientId KorgPadKontrol::deviceId() const
{
    return _impl->deviceId();
}

korgPadKontrol::Program* KorgPadKontrol::program()
{
    return _impl->program();
}

void KorgPadKontrol::setProgram(korgPadKontrol::Program* prog)
{
    const auto* old = _impl->program();
    if (old == prog)
        return;

    _impl->setProgram(prog);
    emit programChanged();
}

void KorgPadKontrol::setNativeMode()
{
    if (!_impl->setMode(midi::KorgPadKontrol::Mode::native))
        emit isNativeChanged();
}

void KorgPadKontrol::setNormalMode()
{
    if (!_impl->setMode(midi::KorgPadKontrol::Mode::normal))
        emit isNativeChanged();
}

std::error_code KorgPadKontrol::loadDeviceSceneIntoProgram()
{
    return _impl->loadDeviceSceneIntoProgram();
}

std::error_code KorgPadKontrol::uploadProgramToDevice()
{
    return _impl->uploadProgramToDevice();
}

} // namespace paddock
