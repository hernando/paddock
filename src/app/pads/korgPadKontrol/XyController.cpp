#include "XyController.hpp"

#include "knobHelpers.hpp"

#include "app/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include "utils/overloaded.hpp"

namespace paddock::korgPadKontrol
{
using Scene = midi::korgPadKontrol::Scene;

XyController::XyController(QObject* parent)
    : QObject(parent)
{
}

Program* XyController::program()
{
    return _program;
}

void XyController::setProgram(Program* program)
{
    if (program == _program)
        return;

    _program = program;

    emit programChanged();
}

void XyController::toggleEnabled(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& knob = axisIndex == 0 ? scene.x : scene.y;
    knob.enabled = !knob.enabled;
    _program->resetScene(std::move(scene));
}

void XyController::togglePolarity(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& axis = axisIndex == 0 ? scene.x : scene.y;
    axis.reversePolarity = !axis.reversePolarity;
    _program->resetScene(std::move(scene));
}

void XyController::incrementParameter(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    auto& axis = axisIndex == 0 ? scene.x : scene.y;
    korgPadKontrol::incrementParameter(axis);
    if (axis.type == Scene::KnobType::AfterTouch && axis.releaseValue < -127)
        axis.releaseValue = 0;
    _program->resetScene(std::move(scene));
}

void XyController::decrementParameter(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    korgPadKontrol::decrementParameter(axisIndex == 0 ? scene.x : scene.y);
    _program->resetScene(std::move(scene));
}

void XyController::incrementReleaseValue(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    auto& axis = axisIndex == 0 ? scene.x : scene.y;
    if (axis.releaseValue < 127)
        ++axis.releaseValue;
    _program->resetScene(std::move(scene));
}

void XyController::decrementReleaseValue(int axisIndex)
{
    if (!_program || !_program->hasScene())
        return;
    auto scene = *_program->midiProgram().scene();
    auto& axis = axisIndex == 0 ? scene.x : scene.y;
    switch (axis.type)
    {
    case Scene::KnobType::PitchBend:
        if (axis.releaseValue > -128)
            --axis.releaseValue;
        break;
    case Scene::KnobType::AfterTouch:
    case Scene::KnobType::Controller:
        if (axis.releaseValue > 0)
            --axis.releaseValue;
        break;
    }
    _program->resetScene(std::move(scene));
}

} // namespace paddock::korgPadKontrol
