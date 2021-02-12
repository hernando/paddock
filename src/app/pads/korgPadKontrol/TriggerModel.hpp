#pragma once

#include "ProgramObserver.hpp"

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QAbstractListModel>

namespace paddock::korgPadKontrol
{
class TriggerModel : public QAbstractListModel, public ProgramObserver<TriggerModel>
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

    Q_PROPERTY(int padCount MEMBER padCount CONSTANT)

public:
    static constexpr int padCount = 16;

    enum class Role
    {
        Enabled = Qt::UserRole + 1,
        Port,
        MidiChannel,
        HasFlamRoll,
        ActionType,
        SwitchType,
        Knob1Assigned,
        Knob2Assigned,
        xAxisAssigned,
        yAxisAssigned,
        Note,
        Velocity,
        Parameter,
        Value,
        ReleaseValue
    };

    enum ActionType
    {
        NoteAction = 0,
        ControlAction = 1
    };
    Q_ENUM(ActionType)

    enum SwitchType
    {
        MomentarySwitch = 0,
        ToggleSwitch = 1
    };
    Q_ENUM(SwitchType)

    enum PortName
    {
        PortA = int(midi::korgPadKontrol::Scene::Port::A),
        PortB = int(midi::korgPadKontrol::Scene::Port::B)
    };
    Q_ENUM(PortName)

    explicit TriggerModel(QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

signals:
    void programChanged();

private:
    friend class ProgramObserver<TriggerModel>;

    midi::korgPadKontrol::Scene::Trigger _pads[16];
    std::array<short, 4> _knobAssignmentBits;

    void updateModel();
};

} // namespace paddock::korgPadKontrol
