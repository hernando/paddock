#include "TriggerModel.hpp"

#include "Program.hpp"

#include "midi/noteNames.hpp"
#include "utils/overloaded.hpp"

namespace paddock
{
namespace korgPadKontrol
{
TriggerModel::TriggerModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> TriggerModel::roleNames() const
{
    return {{static_cast<int>(Role::Enabled), "enabled"},
            {static_cast<int>(Role::Port), "port"},
            {static_cast<int>(Role::MidiChannel), "midiChannel"},
            {static_cast<int>(Role::HasFlamRoll), "hasFlamRoll"},
            {static_cast<int>(Role::ActionType), "actionType"},
            {static_cast<int>(Role::SwitchType), "switchType"},
            {static_cast<int>(Role::Note), "note"},
            {static_cast<int>(Role::Velocity), "velocity"},
            {static_cast<int>(Role::Parameter), "parameter"},
            {static_cast<int>(Role::Value), "value"},
            {static_cast<int>(Role::ReleaseValue), "releaseValue"}};
}

int TriggerModel::rowCount(const QModelIndex&) const
{
    return padCount;
}

QVariant TriggerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto& trigger = _pads[static_cast<size_t>(index.row())];

    using SceneNote = midi::korgPadKontrol::Scene::Note;
    using Control = midi::korgPadKontrol::Scene::Control;

    const auto isNote = std::holds_alternative<SceneNote>(trigger.action);

    switch (static_cast<Role>(role))
    {
    case Role::Enabled:
        return QVariant::fromValue(trigger.enabled);
    case Role::Port:
        return QVariant::fromValue(static_cast<int>(trigger.port));
    case Role::MidiChannel:
        return QVariant::fromValue(trigger.midiChannel);
    case Role::HasFlamRoll:
        return QVariant::fromValue(trigger.hasFlamRoll);
    case Role::ActionType:
        return QVariant::fromValue(
            static_cast<int>(isNote ? NoteAction : ControlAction));
    case Role::SwitchType:
        return QVariant::fromValue(static_cast<int>(trigger.type));
    case Role::Note:
        return isNote ? QVariant::fromValue(QString{
                            midi::noteNames[std::get<SceneNote>(trigger.action)
                                                .note]})
                      : QVariant{};
    case Role::Velocity:
        return isNote
                   ? QVariant::fromValue(std::visit(
                         overloaded{
                             [](midi::Value7bit velocity) {
                                 return static_cast<int>(velocity);
                             },
                             [](midi::korgPadKontrol::Scene::Note::VelocityCurve
                                    curve) {
                                 return -static_cast<int>(curve) - 1;
                             }},
                         std::get<SceneNote>(trigger.action).velocity))
                   : QVariant{};
    case Role::Parameter:
        return isNote ? QVariant{}
                      : QVariant::fromValue(static_cast<int>(
                            std::get<Control>(trigger.action).param));
    case Role::Value:
        return isNote ? QVariant{}
                      : QVariant::fromValue(static_cast<int>(
                            std::get<Control>(trigger.action).value));
    case Role::ReleaseValue:
        return isNote ? QVariant{}
                      : QVariant::fromValue(static_cast<int>(
                            std::get<Control>(trigger.action).releaseValue));
    default:
        break;
    }
    throw std::range_error("Invalid role");
}

Program* TriggerModel::program()
{
    return _program;
}

void TriggerModel::setProgram(Program* program)
{
    if (program == _program)
        return;

    if (_program)
        _program->disconnect(this);

    _program = program;
    connect(_program, &paddock::Program::changed, this,
            &TriggerModel::updateModel);

    updateModel();

    emit programChanged();
}

void TriggerModel::updateModel()
{
    if (!_program || !_program->hasScene())
        return; // TODO reset to default
    const auto& scene = *_program->midiProgram().scene();
    for (int i = 0; i != 16; ++i)
    {
        if (_pads[i] != scene.pads[i])
        {
            _pads[i] = scene.pads[i];
            emit dataChanged(index(i, 0), index(i, 0));
        }
    }
}

} // namespace korgPadKontrol
} // namespace paddock
