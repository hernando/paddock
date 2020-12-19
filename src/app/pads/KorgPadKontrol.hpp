#pragma once

#include "models.hpp"

#include "midi/types.hpp"

#include <QObject>

#include <memory>

namespace paddock
{
namespace midi
{
class KorgPadKontrol;
}

class KorgPadKontrolProgram;

class KorgPadKontrol : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isNative READ isNative NOTIFY isNativeChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(paddock::ControllerModel::Model model MEMBER model CONSTANT)
    Q_PROPERTY(paddock::KorgPadKontrolProgram* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    static constexpr ControllerModel::Model model =
        ControllerModel::KorgPadKontrol;

    KorgPadKontrol(QObject* parent, midi::KorgPadKontrol&& controller);
    ~KorgPadKontrol();

    bool isNative() const;
    bool isConnected() const;

    /// To be called from session when the device has been detected to be
    /// be disconnected
    void disconnect();

    std::error_code setController(midi::KorgPadKontrol&& controller);
    midi::ClientId deviceId() const;

    KorgPadKontrolProgram* program();
    void setProgram(KorgPadKontrolProgram* prog);

public Q_SLOTS:
    void setNativeMode();
    void setNormalMode();
    void dumpCurrentScene();

signals:
    void isNativeChanged();
    void isConnectedChanged();
    void programChanged();

private:
    class _Impl;
    // A shared ptr is needed to capture it in the callbacks passed
    // to the poller.
    std::unique_ptr<_Impl> _impl;
};

} // namespace paddock
