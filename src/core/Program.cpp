#include "Program.hpp"

namespace paddock
{
namespace core
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

} // namespace core
} // namespace paddock
