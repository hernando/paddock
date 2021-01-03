#pragma once

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QObject>

namespace paddock
{
namespace korgPadKontrol
{
class Program;

class TriggerController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    explicit TriggerController(QObject* parent = nullptr);

    Program* program();
    void setProgram(Program* program);

public Q_SLOTS:
    void toggleEnabled(int pad);
    void togglePort(int pad);
    void toggleSwitchType(int pad);
    void toggleFlamRoll(int pad);
    void toggleKnobAssignment(int pad, int knob);

    void incrementMidiChannel(int pad);
    void decrementMidiChannel(int pad);

    void incrementActionValue(int pad);
    void decrementActionValue(int pad);

    void incrementVelocity(int pad);
    void decrementVelocity(int pad);

    void incrementValue(int pad);
    void decrementValue(int pad);

    void incrementReleaseValue(int pad);
    void decrementReleaseValue(int pad);

signals:
    void programChanged();

private:
    Program* _program{nullptr};
};

} // namespace korgPadKontrol
} // namespace paddock
