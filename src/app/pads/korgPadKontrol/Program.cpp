#include "Program.hpp"

namespace paddock
{
namespace korgPadKontrol
{
Program::Program(QObject* parent)
    : paddock::Program(parent)
{
}

const midi::korgPadKontrol::Program& Program::midiProgram() const
{
    return _program;
}

bool Program::hasScene() const
{
    return _program.scene() != nullptr;
}

void Program::resetScene(midi::korgPadKontrol::Scene scene)
{
    _program.setScene(std::move(scene));
    emit changed();
}

} // namespace korgPadKontrol
} // namespace paddock
