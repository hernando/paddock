#pragma once

#include <QObject>

#include "utils/Expected.hpp"

namespace paddock
{

class Program : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)

public:
    Program(QObject* parent);

    bool dirty() const
    {
        return _dirty;
    }

    void setDirty(bool dirty);

    virtual QByteArray serialize() const = 0;
    static Expected<Program*> unserialize(QObject* parent, QByteArray bytes);

signals:
    void dirtyChanged();
    void changed();

private:
    bool _dirty{false};
};

} // namespace paddock
