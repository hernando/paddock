#pragma once

#include "app/Program.hpp"

#include "midi/pads/korgPadKontrol/Program.hpp"

namespace paddock
{
class KorgPadKontrolProgram : public Program
{
    Q_OBJECT

public:
    KorgPadKontrolProgram(QObject* parent);

    const midi::korgPadKontrol::Program& midiProgram() const;
    bool hasScene() const;

    // Reset the scene without triggering the programChanged signal
    void resetScene(midi::korgPadKontrol::Scene scene);

signals:
    void programChanged();

private:
    midi::korgPadKontrol::Program program;
};

} // namespace paddock
