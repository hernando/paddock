#pragma once

#include "ProgramObserver.hpp"

#include "KnobModel.hpp"

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QAbstractListModel>

namespace paddock::korgPadKontrol
{
class XyModel : public QAbstractListModel, public ProgramObserver<XyModel>
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    static constexpr int knobCount = 2;

    enum class Role
    {
        Enabled = Qt::UserRole + 1,
        Name,
        ActionType,
        Parameter,
        ReversePolarity,
        ReleaseValue,
    };

    using ActionType = KnobModel::ActionType;

    explicit XyModel(QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

signals:
    void programChanged();

private:
    friend class ProgramObserver<XyModel>;

    midi::korgPadKontrol::Scene::Axis _axes[2];

    void updateModel();
};

} // namespace paddock::korgPadKontrol
