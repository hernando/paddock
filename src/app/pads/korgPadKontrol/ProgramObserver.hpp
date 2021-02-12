#pragma once

#include "Program.hpp"

#include "midi/pads/korgPadKontrol/Scene.hpp"

#include <QAbstractListModel>

namespace paddock::korgPadKontrol
{
class Scene;

template <typename Derived>
class ProgramObserver
{
public:
    Program* program()
    {
        return _program;
    }

    void setProgram(Program* program)
    {
        if (program == _program)
            return;

        if (_program)
            _program->disconnect(asQObject());

        _program = program;
        QObject::connect(_program, &paddock::Program::changed, asQObject(),
                         &Derived::updateModel);

        asQObject()->updateModel();

        Q_EMIT asQObject()->programChanged();
    }

    bool hasScene() const
    {
        return _program && _program->hasScene();
    }

private:
    Program* _program{nullptr};

    Derived* asQObject()
    {
        return static_cast<Derived*>(this);
    }
};

} // namespace paddock::korgPadKontrol
