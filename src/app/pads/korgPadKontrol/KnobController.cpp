#include "KnobController.hpp"

#include "app/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include "utils/overloaded.hpp"

namespace paddock::korgPadKontrol
{
KnobController::KnobController(QObject* parent)
    : QObject(parent)
{
}

Program* KnobController::program()
{
    return _program;
}

void KnobController::setProgram(Program* program)
{
    if (program == _program)
        return;

    _program = program;

    emit programChanged();
}

void KnobController::toggleEnabled(int knobIndex)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& knob = scene.knobs[knobIndex];
    knob.enabled = !knob.enabled;
    _program->resetScene(std::move(scene));
}

} // namespace paddock::korgPadKontrol
