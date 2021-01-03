#include "TriggerController.hpp"

#include "app/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

#include "utils/overloaded.hpp"

#include <iostream>

namespace paddock
{
namespace korgPadKontrol
{
using Value7bit = midi::Value7bit;
using Scene = midi::korgPadKontrol::Scene;
using Action = std::variant<Scene::Note, Scene::Control>;
using Curve = Scene::Note::VelocityCurve;
using Velocity = std::variant<Curve, Value7bit>;

namespace
{
template <typename T>
T toggle(T value)
{
    return static_cast<T>(1 - static_cast<int>(value));
}

Scene::Trigger& getTrigger(Scene& scene, int index)
{
    if (index > 16)
        throw std::logic_error("Index out of bounds");
    if (index == 16)
        return scene.pedal;
    return scene.pads[index];
}
}

TriggerController::TriggerController(QObject* parent)
    : QObject(parent)
{
}

Program* TriggerController::program()
{
    return _program;
}

void TriggerController::setProgram(Program* program)
{
    if (program == _program)
        return;

    _program = program;

    emit programChanged();
}

void TriggerController::toggleEnabled(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.enabled = !trigger.enabled;
    _program->resetScene(std::move(scene));
}

void TriggerController::togglePort(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.port = toggle(trigger.port);
    _program->resetScene(std::move(scene));
}

void TriggerController::toggleSwitchType(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.type = toggle(trigger.type);
    _program->resetScene(std::move(scene));
}

void TriggerController::toggleFlamRoll(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.hasFlamRoll = !trigger.hasFlamRoll;
    _program->resetScene(std::move(scene));
}

void TriggerController::toggleKnobAssignment(int pad, int knobIndex)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    if (knobIndex > 1)
        throw std::logic_error("Index out of bounds");
    auto& knob = scene.knobs[knobIndex];
    if (pad == 16)
        knob.pedalAssigned = !knob.pedalAssigned;
    else
        knob.padAssignmentBits ^= 1 << pad;
    _program->resetScene(std::move(scene));
}

void TriggerController::incrementMidiChannel(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.midiChannel =
        std::min(16, std::max(1, trigger.midiChannel + 1));
    _program->resetScene(std::move(scene));
}

void TriggerController::decrementMidiChannel(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.midiChannel =
        std::min(16, std::max(1, trigger.midiChannel - 1));
    _program->resetScene(std::move(scene));
}

void TriggerController::incrementActionValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action =
        std::visit(overloaded{[](Scene::Control control) -> Action {
                                  control.param =
                                      std::min(127, control.param + 1);
                                  return control;
                              },
                              [](Scene::Note note) -> Action {
                                  if (note.note == 127)
                                      return Scene::Control{0, 127, 0};
                                  ++note.note;
                                  return note;
                              }},
                   trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::decrementActionValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action = std::visit(
        overloaded{
            [](Scene::Control control) -> Action {
                if (control.param == 0)
                    return Scene::Note{127, Scene::Note::VelocityCurve::curve1};
                --control.param;
                return control;
            },
            [](Scene::Note note) -> Action {
                note.note = std::max(0, note.note - 1);
                return note;
            }},
        trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::incrementVelocity(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action = std::visit(
        overloaded{[](Scene::Control control) -> Action { return control; },
                   [](Scene::Note note) -> Action {
                       note.velocity = std::visit(
                           overloaded{
                               [](Curve curve) -> Velocity {
                                   if (curve == Curve::curve8)
                                       return Value7bit{1};
                                   return Curve{static_cast<int>(curve) + 1};
                               },
                               [](Value7bit value) -> Velocity {
                                   return std::min(Value7bit{127}, ++value);
                               }},
                           note.velocity);
                       return note;
                   }},
        trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::decrementVelocity(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action = std::visit(
        overloaded{[](Scene::Control control) -> Action { return control; },
                   [pad](Scene::Note note) -> Action {
                       note.velocity = std::visit(
                           overloaded{[](Curve curve) -> Velocity {
                                          if (curve == Curve::curve1)
                                              return curve;
                                          return Curve{static_cast<int>(curve) -
                                                       1};
                                      },
                                      [pad](Value7bit value) -> Velocity {
                                          if (value == 1 && pad == 16)
                                              return Value7bit{1};
                                          if (value == 1)
                                              return Curve::curve8;
                                          return --value;
                                      }},
                           note.velocity);
                       return note;
                   }},
        trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::incrementValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action =
        std::visit(overloaded{[](Scene::Control control) -> Action {
                                  control.value =
                                      std::min(127, control.value + 1);
                                  return control;
                              },
                              [](Scene::Note note) -> Action { return note; }},
                   trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::decrementValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action =
        std::visit(overloaded{[](Scene::Control control) -> Action {
                                  control.value =
                                      std::max(0, control.value - 1);
                                  return control;
                              },
                              [](Scene::Note note) -> Action { return note; }},
                   trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::incrementReleaseValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action =
        std::visit(overloaded{[](Scene::Control control) -> Action {
                                  control.releaseValue =
                                      std::min(127, control.releaseValue + 1);

                                  return control;
                              },
                              [](Scene::Note note) -> Action { return note; }},
                   trigger.action);
    _program->resetScene(std::move(scene));
}

void TriggerController::decrementReleaseValue(int pad)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& trigger = getTrigger(scene, pad);
    trigger.action =
        std::visit(overloaded{[](Scene::Control control) -> Action {
                                  control.releaseValue =
                                      std::max(0, control.releaseValue - 1);

                                  return control;
                              },
                              [](Scene::Note note) -> Action { return note; }},
                   trigger.action);
    _program->resetScene(std::move(scene));
}

} // namespace korgPadKontrol
} // namespace paddock
