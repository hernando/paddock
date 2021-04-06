#pragma once

#include "app/Program.hpp"

#include "midi/pads/korgPadKontrol/Program.hpp"

namespace paddock::korgPadKontrol
{
class Program : public paddock::Program
{
    Q_OBJECT

public:
    Program(QObject* parent);

    const midi::korgPadKontrol::Program& midiProgram() const;
    bool hasScene() const;

    // Reset the scene without triggering the programChanged signal
    void resetScene(midi::korgPadKontrol::Scene scene);

    QByteArray serialize() const;
    std::error_code unserialize(QByteArray bytes);

private:
    midi::korgPadKontrol::Program _program;
};

} // namespace paddock::korgPadKontrol
