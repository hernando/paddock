#include "KnobController.hpp"

#include "knobHelpers.hpp"

#include "app/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include "utils/overloaded.hpp"

namespace paddock::korgPadKontrol
{
using Scene = midi::korgPadKontrol::Scene;

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

void KnobController::togglePolarity(int knobIndex)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& knob = scene.knobs[knobIndex];
    knob.reversePolarity = !knob.reversePolarity;
    _program->resetScene(std::move(scene));
}

void KnobController::incrementParameter(int knobIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    korgPadKontrol::incrementParameter(scene.knobs[knobIndex]);
    _program->resetScene(std::move(scene));
}

void KnobController::decrementParameter(int knobIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    korgPadKontrol::decrementParameter(scene.knobs[knobIndex]);
    _program->resetScene(std::move(scene));
}

} // namespace paddock::korgPadKontrol
