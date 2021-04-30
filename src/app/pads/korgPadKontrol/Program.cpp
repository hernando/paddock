#include "Program.hpp"

#include "io/Serializer.hpp"

namespace paddock::korgPadKontrol
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
    setDirty(true);
}

std::string Program::serialize() const
{
    return "{}";
}

std::error_code Program::deserialize(const std::string& bytes)
{
    (void)bytes;
    return std::error_code();
}

} // namespace paddock::korgPadKontrol
