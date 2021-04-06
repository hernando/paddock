#include "Program.hpp"

namespace paddock
{
Program::Program(QObject* parent)
    : QObject(parent)
{
}

void Program::setDirty(bool dirty)
{
    const bool changed = _dirty != dirty;
    _dirty = dirty;
    if (changed)
        Q_EMIT dirtyChanged();
}

Expected<Program*> Program::unserialize(QObject* parent, QByteArray bytes)
{
    return nullptr;
}

} // namespace paddock
