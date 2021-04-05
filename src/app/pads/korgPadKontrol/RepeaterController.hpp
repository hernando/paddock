#pragma once

#include "RepeaterModel.hpp"

#include <QObject>

namespace paddock::korgPadKontrol
{
class Program;

class RepeaterController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(paddock::korgPadKontrol::Program* program READ program WRITE
                   setProgram NOTIFY programChanged)

public:
    explicit RepeaterController(QObject* parent = nullptr);

    Program* program();
    void setProgram(Program* program);

public Q_SLOTS:
    void incrementParameter(int repeater, int parameter);
    void decrementParameter(int repeater, int parameter);

signals:
    void programChanged();

private:
    Program* _program{nullptr};
};

} // namespace paddock::korgPadKontrol
