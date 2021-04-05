#include "XyModel.hpp"

namespace paddock::korgPadKontrol
{
XyModel::XyModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> XyModel::roleNames() const
{
    return {{static_cast<int>(Role::Enabled), "enabled"},
            {static_cast<int>(Role::Name), "name"},
            {static_cast<int>(Role::ActionType), "type"},
            {static_cast<int>(Role::Parameter), "parameter"},
            {static_cast<int>(Role::ReversePolarity), "reversePolarity"},
            {static_cast<int>(Role::ReleaseValue), "releaseValue"}};
}

int XyModel::rowCount(const QModelIndex& parent) const
{
    return 2;
}

QVariant XyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto axisIndex = static_cast<size_t>(index.row());
    const auto& axis = _axes[axisIndex];

    switch (static_cast<Role>(role))
    {
    case Role::Enabled:
        return QVariant::fromValue(axis.enabled);
    case Role::Name:
        return axisIndex == 0 ? "X axis" : "Y axis";
    case Role::ActionType:
        return QVariant::fromValue(static_cast<int>(axis.type));
    case Role::Parameter:
        return QVariant::fromValue(axis.param);
    case Role::ReversePolarity:
        return QVariant::fromValue(axis.reversePolarity);
    case Role::ReleaseValue:
        return QVariant::fromValue(axis.releaseValue);
    default:
        break;
    }
    throw std::range_error("Invalid role");
}

void XyModel::updateModel()
{
    if (!hasScene())
        return; // TODO reset to default
    const auto& scene = *program()->midiProgram().scene();

    _axes[0] = scene.x;
    _axes[1] = scene.y;
    emit dataChanged(index(0, 0), index(1, 0));
}

} // namespace paddock::korgPadKontrol
