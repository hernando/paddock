#include "RepeaterController.hpp"

#include "app/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Program.hpp"
#include "midi/pads/korgPadKontrol/Scene.hpp"

namespace paddock::korgPadKontrol
{
namespace
{
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept
{
    return static_cast<unsigned char>(arg);
}

template <auto max = 0xffffffffffffull, typename T>
void increment(T& parameter)
{
    if (parameter == static_cast<T>(max))
        return;
    ++parameter;
}

template <auto min = 0, typename T>
void decrement(T& parameter)
{
    if (parameter <= static_cast<T>(min))
        return;
    --parameter;
}
} // namespace

RepeaterController::RepeaterController(QObject* parent)
    : QObject(parent)
{
}

Program* RepeaterController::program()
{
    return _program;
}

void RepeaterController::setProgram(Program* program)
{
    if (program == _program)
        return;

    _program = program;

    emit programChanged();
}

void RepeaterController::incrementParameter(int index, int parameter)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& repeater = index == 0 ? scene.flam : scene.roll;

    switch (static_cast<RepeaterModel::Role>(parameter))
    {
    case RepeaterModel::Role::MinSpeed:
        if (index == 1)
            increment<240>(repeater.minSpeed);
        else
            increment(repeater.minSpeed);
        break;
    case RepeaterModel::Role::MaxSpeed:
        if (index == 1)
            increment<240>(repeater.maxSpeed);
        else
            increment(repeater.maxSpeed);
        break;
    case RepeaterModel::Role::MinVolume:
        increment<127>(repeater.minVolume);
        break;
    case RepeaterModel::Role::MaxVolume:
        increment<127>(repeater.maxVolume);
        break;
    default:
        throw std::logic_error("Invalid parameter");
    }

    _program->resetScene(std::move(scene));
}

void RepeaterController::decrementParameter(int index, int parameter)
{
    if (!_program || !_program->hasScene())
        return;

    auto scene = *_program->midiProgram().scene();
    auto& repeater = index == 0 ? scene.flam : scene.roll;

    switch (static_cast<RepeaterModel::Role>(parameter))
    {
    case RepeaterModel::Role::MinSpeed:
        if (index == 1)
            decrement<40>(repeater.minSpeed);
        else
            decrement(repeater.minSpeed);
        break;
    case RepeaterModel::Role::MaxSpeed:
        if (index == 1)
            decrement<40>(repeater.maxSpeed);
        else
            decrement(repeater.maxSpeed);
        break;
    case RepeaterModel::Role::MinVolume:
        decrement<1>(repeater.minVolume);
        break;
    case RepeaterModel::Role::MaxVolume:
        decrement<1>(repeater.maxVolume);
        break;
    default:
        throw std::logic_error("Invalid parameter");
    }
    _program->resetScene(std::move(scene));
}

} // namespace paddock::korgPadKontrol
