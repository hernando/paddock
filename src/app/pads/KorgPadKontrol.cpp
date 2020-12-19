#include "KorgPadKontrol.hpp"

#include "KorgPadKontrolProgram.hpp"

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
    _Impl(KorgPadKontrol* parent, midi::KorgPadKontrol&& controller)
        : _parent{parent}
        , _controller{std::move(controller)}
        , _program{nullptr}
    {
        setProgram(new KorgPadKontrolProgram{parent});
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

    KorgPadKontrolProgram* program() { return _program; }

    void setProgram(KorgPadKontrolProgram* prog)
    {
        if (_program)
            _program->disconnect(_parent);

        _program = prog;
        if (_program)
        {
            _program->connect(_program, &KorgPadKontrolProgram::programChanged,
                              _parent, [this]() {
                                  if (auto error = _getOrSetScene())
                                      core::log() << error.message();
                              });
        }

        _getOrSetScene();
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

    Expected<midi::korgPadKontrol::Scene> queryCurrentScene()
    {
        if (!_controller)
            return tl::unexpected(midi::EngineError::noDeviceFound);
        return _controller->queryCurrentScene();
    }

private:
    KorgPadKontrol* _parent;
    std::optional<midi::KorgPadKontrol> _controller;

    KorgPadKontrolProgram* _program;
};

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
    return std::error_code{};
    emit isConnectedChanged();
}

midi::ClientId KorgPadKontrol::deviceId() const
{
    return _impl->deviceId();
}

KorgPadKontrolProgram* KorgPadKontrol::program()
{
    return _impl->program();
}

void KorgPadKontrol::setProgram(KorgPadKontrolProgram* prog)
{
    KorgPadKontrolProgram* old = _impl->program();
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

void KorgPadKontrol::dumpCurrentScene()
{
    _impl->queryCurrentScene();
}

} // namespace paddock
