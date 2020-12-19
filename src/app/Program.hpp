#pragma once

#include <QObject>

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

signals:
    void dirtyChanged();

private:
    bool _dirty{false};
};

} // namespace paddock
