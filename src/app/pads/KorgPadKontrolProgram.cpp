#include "KorgPadKontrolProgram.hpp"

namespace paddock
{
KorgPadKontrolProgram::KorgPadKontrolProgram(QObject* parent)
    : Program(parent)
{
}

const midi::korgPadKontrol::Program& KorgPadKontrolProgram::midiProgram() const
{
    return program;
}

bool KorgPadKontrolProgram::hasScene() const
{
    return program.scene() != nullptr;
}

void KorgPadKontrolProgram::resetScene(midi::korgPadKontrol::Scene scene)
{
    program.setScene(std::move(scene));
}

} // namespace paddock
