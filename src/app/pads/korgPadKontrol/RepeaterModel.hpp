#pragma once

#include "ProgramObserver.hpp"

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QAbstractListModel>

namespace paddock::korgPadKontrol
{
class RepeaterModel : public QAbstractListModel, public ProgramObserver<RepeaterModel>
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

    Q_PROPERTY(int repeaterCount MEMBER repeaterCount CONSTANT)

public:
    static constexpr int repeaterCount = 2;

    enum class Role
    {
        Name,
        MinSpeed,
        MaxSpeed,
        MinVolume,
        MaxVolume
    };
    Q_ENUM(Role)

    explicit RepeaterModel(QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

signals:
    void programChanged();

private:
    friend class ProgramObserver<RepeaterModel>;

    midi::korgPadKontrol::Scene::Repeater _repeaters[repeaterCount];

    void updateModel();
};

} // namespace paddock::korgPadKontrol
