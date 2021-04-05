#pragma once

#include "midi/pads/korgPadKontrol/Scene.hpp"

namespace paddock::korgPadKontrol
{
inline void incrementParameter(midi::korgPadKontrol::Scene::Knob& knob)
{
    using Scene = midi::korgPadKontrol::Scene;

    switch (knob.type)
    {
    case Scene::KnobType::PitchBend:
        knob.type = Scene::KnobType::AfterTouch;
        break;
    case Scene::KnobType::AfterTouch:
        knob.type = Scene::KnobType::Controller;
        knob.param = 0;
        break;
    case Scene::KnobType::Controller:
        if (knob.param == 127)
            return;
        else
            ++knob.param;
        break;
    }
}

inline void decrementParameter(midi::korgPadKontrol::Scene::Knob& knob)
{
    using Scene = midi::korgPadKontrol::Scene;

    switch (knob.type)
    {
    case Scene::KnobType::PitchBend:
        return;
    case Scene::KnobType::AfterTouch:
        knob.type = Scene::KnobType::PitchBend;
        break;
    case Scene::KnobType::Controller:
        if (knob.param == 0)
            knob.type = Scene::KnobType::AfterTouch;
        else
            --knob.param;
        break;
    }
}
}
