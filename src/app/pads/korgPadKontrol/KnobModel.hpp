#pragma once

#include "ProgramObserver.hpp"

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QAbstractListModel>

namespace paddock::korgPadKontrol
{
class KnobModel : public QAbstractListModel, public ProgramObserver<KnobModel>
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

    Q_PROPERTY(int knobCount MEMBER knobCount CONSTANT)

public:
    static constexpr int knobCount = 2;

    enum class Role
    {
        Enabled = Qt::UserRole + 1,
        KnobType,
        Parameter,
        ReversePolarity,
    };

    using KnobType = midi::korgPadKontrol::Scene::KnobType;
    Q_ENUM(KnobType)

    explicit KnobModel(QObject* parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

signals:
    void programChanged();

private:
    friend class ProgramObserver<KnobModel>;

    midi::korgPadKontrol::Scene::Knob _knobs[knobCount];

    void updateModel();
};

} // namespace paddock::korgPadKontrol
