#include "KorgPadKontrol.hpp"

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
        : _parent(parent)
        , _controller(std::move(controller))
    {
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

    void setController(midi::KorgPadKontrol&& controller)
    {
        _controller = std::move(controller);
    }

    midi::ClientId deviceId() const
    {
        if (!_controller)
            return midi::ClientId{};
        return _controller->deviceId();
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
    _impl->setController(std::move(controller));
    emit isConnectedChanged();

    // This function may fail when applying the program
    return std::error_code{};
}

midi::ClientId KorgPadKontrol::deviceId() const
{
    return _impl->deviceId();
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
