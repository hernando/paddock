#include "KorgPadKontrol.hpp"

#include "midi/pads/KorgPadKontrol.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QObject>

#include <iostream>

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

    bool isNative()
    {
        return _controller.mode() == midi::KorgPadKontrol::Mode::native;
    }

    std::error_code setMode(midi::KorgPadKontrol::Mode mode)
    {
        return _controller.setMode(mode);
    }

    Expected<midi::korgPadKontrol::Scene> queryCurrentScene()
    {
        return _controller.queryCurrentScene();
    }

private:
    KorgPadKontrol* _parent;
    midi::KorgPadKontrol _controller;
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
