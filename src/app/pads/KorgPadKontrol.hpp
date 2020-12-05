#pragma once

#include "models.hpp"

#include <QObject>

#include <memory>

namespace paddock
{
namespace midi
{
class KorgPadKontrol;
}

class KorgPadKontrol : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isNative READ isNative NOTIFY isNativeChanged)
    Q_PROPERTY(paddock::ControllerModel::Model model MEMBER model CONSTANT)

public:
    static constexpr ControllerModel::Model model = ControllerModel::KorgPadKontrol;

    KorgPadKontrol(QObject* parent, midi::KorgPadKontrol&& controller);
    ~KorgPadKontrol();

    bool isNative() const;

public Q_SLOTS:
    void setNativeMode();
    void setNormalMode();

signals:
    void isNativeChanged();

private:
    class _Impl;
    // A shared ptr is needed to capture it in the callbacks passed
    // to the poller.
    std::unique_ptr<_Impl> _impl;
};

}

