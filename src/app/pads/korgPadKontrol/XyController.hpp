#pragma once

#include <QObject>

namespace paddock::korgPadKontrol
{
class Program;

class XyController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    explicit XyController(QObject* parent = nullptr);

    Program* program();
    void setProgram(Program* program);

public Q_SLOTS:
    void toggleEnabled(int axis);
    void togglePolarity(int axis);
    void incrementParameter(int axis);
    void decrementParameter(int axis);
    void incrementReleaseValue(int axis);
    void decrementReleaseValue(int axis);

signals:
    void programChanged();

private:
    Program* _program{nullptr};
};

} // namespace paddock::korgPadKontrol
