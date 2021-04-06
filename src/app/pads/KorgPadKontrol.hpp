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

namespace korgPadKontrol
{
class Program;
}

class KorgPadKontrol : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isNative READ isNative NOTIFY isNativeChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(paddock::ControllerModel::Model model MEMBER model CONSTANT)
    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    static constexpr ControllerModel::Model model =
        ControllerModel::KorgPadKontrol;
    using MidiController = midi::KorgPadKontrol;

    KorgPadKontrol(QObject* parent);
    KorgPadKontrol(QObject* parent, midi::KorgPadKontrol&& controller);
    ~KorgPadKontrol();

    bool isNative() const;
    bool isConnected() const;

    /// To be called from session when the device has been detected to be
    /// be disconnected
    void disconnect();

    std::error_code setController(midi::KorgPadKontrol&& controller);
    midi::ClientId deviceId() const;

    korgPadKontrol::Program* program();
    void setProgram(korgPadKontrol::Program* prog);

public Q_SLOTS:
    void setNativeMode();
    void setNormalMode();
    std::error_code loadDeviceSceneIntoProgram();
    std::error_code uploadProgramToDevice();

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
