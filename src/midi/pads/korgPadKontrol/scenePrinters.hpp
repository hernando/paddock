#include "Scene.hpp"

#include "midi/noteNames.hpp"
#include "utils/overloaded.hpp"

#include <iostream>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
std::ostream& operator<<(std::ostream& out, const Scene::Trigger& trigger)
{
    out << (trigger.enabled ? 'e' : 'd') << int(trigger.midiChannel);
    out << (trigger.type == Scene::SwitchType::Toggle ? 't' : 'm');
    out << (trigger.port == Scene::Port::A ? 'A' : 'B') << ' ';
    out << (trigger.hasFlamRoll ? "w/flam&roll " : "");
    std::visit(
        overloaded{
            [&out](const Scene::Note& note) {
                out << noteNames[note.note] << ' ';
                std::visit(overloaded{[&out](Value7bit v) { out << int(v); },
                                      [&out](Scene::Note::VelocityCurve c) {
                                          out << "C-" << int(c) + 1;
                                      }},
                           note.velocity);
            },
            [&out](const Scene::Control& control) {
                out << "CC#" << int(control.param) << ' ' << int(control.value)
                    << ' ' << int(control.releaseValue);
            }},
        trigger.action);
    return out;
}

std::ostream& operator<<(std::ostream& out, const Scene::Knob& knob)
{
    out << (knob.enabled ? 'e' : 'd');
    out << (knob.reversePolarity ? '-' : '+');
    switch (knob.type)
    {
    case Scene::KnobType::AfterTouch:
        out << 'a';
        break;
    case Scene::KnobType::PitchBend:
        out << 'b';
        break;
    case Scene::KnobType::Controller:
        out << 'c';
        break;
    }
    out << ' ' << int(knob.param);
    printf(" %.4X ", int(knob.padAssignmentBits));
    out << (knob.pedalAssigned ? 'p' : '_');
    return out;
}

std::ostream& operator<<(std::ostream& out, const Scene::Axis& axis)
{
    out << static_cast<const Scene::Knob&>(axis) << " "
        << int(axis.releaseValue);
    return out;
}

std::ostream& operator<<(std::ostream& out, const Scene::Repeater& repeater)
{
    out << "speed " << int(repeater.minSpeed) << '-' << int(repeater.maxSpeed)
        << " volume " << int(repeater.minVolume) << '-'
        << int(repeater.maxVolume);
    return out;
}

std::ostream& operator<<(std::ostream& out, const Scene& scene)
{
    for (size_t i = 0; i != 16; ++i)
    {
        out << "Pad " << (i + 1) << " " << scene.pads[i] << std::endl;
    }
    out << "Pedal " << scene.pedal << std::endl;
    out << "Knob 1 " << scene.knobs[0] << std::endl;
    out << "Knob 2 " << scene.knobs[1] << std::endl;
    out << "X " << scene.x << std::endl;
    out << "Y " << scene.y << std::endl;
    out << "Flam " << scene.flam << std::endl;
    out << "Roll " << scene.roll << std::endl;
    out << "Fixed velocity " << int(scene.fixedVelocity) << std::endl;
    out << std::endl;
    return out;
}

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
