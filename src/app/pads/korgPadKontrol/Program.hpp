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

    std::string serialize() const;
    std::error_code deserialize(const std::string& bytes);

private:
    midi::korgPadKontrol::Program _program;
};

} // namespace paddock::korgPadKontrol
