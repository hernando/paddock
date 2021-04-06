#include "RepeaterModel.hpp"

namespace paddock::korgPadKontrol
{
RepeaterModel::RepeaterModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> RepeaterModel::roleNames() const
{
    return {{static_cast<int>(Role::Name), "name"},
            {static_cast<int>(Role::MinSpeed), "minSpeed"},
            {static_cast<int>(Role::MaxSpeed), "maxSpeed"},
            {static_cast<int>(Role::MinVolume), "minVolume"},
            {static_cast<int>(Role::MaxVolume), "maxVolume"}};
}

int RepeaterModel::rowCount(const QModelIndex&) const
{
    return repeaterCount;
}

QVariant RepeaterModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto repeaterIndex = static_cast<size_t>(index.row());
    const auto& repeater = _repeaters[repeaterIndex];

    switch (static_cast<Role>(role))
    {
    case Role::Name:
        return repeaterIndex == 0 ? tr("Flam") : tr("Roll");
    case Role::MinSpeed:
        return QVariant::fromValue(repeater.minSpeed);
    case Role::MaxSpeed:
        return QVariant::fromValue(repeater.maxSpeed);
    case Role::MinVolume:
        return QVariant::fromValue(repeater.minVolume);
    case Role::MaxVolume:
        return QVariant::fromValue(repeater.maxVolume);
    default:
        break;
    }
    throw std::range_error("Invalid role");
}

void RepeaterModel::updateModel()
{
    if (!hasScene())
        return; // TODO reset to default
    const auto& scene = *program()->midiProgram().scene();

    _repeaters[0] = scene.flam;
    _repeaters[1] = scene.roll;
    emit dataChanged(index(0, 0), index(1, 0));
}

} // namespace paddock::korgPadKontrol
