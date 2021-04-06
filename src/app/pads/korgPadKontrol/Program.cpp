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

QByteArray Program::serialize() const
{
    QByteArray bytes;
    return bytes;
}

std::error_code Program::unserialize(QByteArray bytes)
{
    (void)bytes;
    return std::error_code();
}

} // namespace paddock::korgPadKontrol
