#include "KnobModel.hpp"

namespace paddock::korgPadKontrol
{
KnobModel::KnobModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> KnobModel::roleNames() const
{
    return {{static_cast<int>(Role::Enabled), "enabled"},
            {static_cast<int>(Role::KnobType), "type"},
            {static_cast<int>(Role::Parameter), "paremeter"},
            {static_cast<int>(Role::ReversePolarity), "reservePolarity"}};
}

int KnobModel::rowCount(const QModelIndex& parent) const
{
    return knobCount;
}

QVariant KnobModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto knobIndex = static_cast<size_t>(index.row());
    const auto& knob = _knobs[knobIndex];

    switch (static_cast<Role>(role))
    {
    case Role::Enabled:
        return QVariant::fromValue(knob.enabled);
    case Role::KnobType:
        return QVariant::fromValue(static_cast<int>(knob.type));
    case Role::Parameter:
        return QVariant::fromValue(knob.param);
    case Role::ReversePolarity:
        return QVariant::fromValue(knob.reversePolarity);
    default:
        break;
    }
    throw std::range_error("Invalid role");
}

void KnobModel::updateModel()
{
    if (!hasScene())
        return; // TODO reset to default
    const auto& scene = *program()->midiProgram().scene();

    _knobs[0] = scene.knobs[0];
    _knobs[1] = scene.knobs[1];
}

} // namespace paddock::korgPadKontrol
