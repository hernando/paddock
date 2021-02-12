#pragma once

#include <QObject>

namespace paddock::korgPadKontrol
{
class Program;

class KnobController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    explicit KnobController(QObject* parent = nullptr);

    Program* program();
    void setProgram(Program* program);

public Q_SLOTS:
    void toggleEnabled(int knob);

signals:
    void programChanged();

private:
    Program* _program{nullptr};
};

} // namespace paddock::korgPadKontrol
