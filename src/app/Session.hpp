#pragma once

#include <QObject>
#include <QVariant>

#include <memory>
#include <system_error>

namespace paddock
{
namespace core
{
class Program;
}

namespace midi
{
class Engine;
}

class Session : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QVariant controller READ controller NOTIFY controllerChanged)
    Q_PROPERTY(
        paddock::core::Program* program READ program NOTIFY programChanged)

    Q_PROPERTY(bool isNsmSession READ isNsmSession CONSTANT)

public:
    Session();
    ~Session();

    std::error_code init();

    core::Program* program() const;

    QVariant controller();

    bool isNsmSession() const;

    /// Name to use for the JACK client
    const std::string& name() const;

    std::error_code openProgram(const std::string& filePath);


signals:
    void programChanged();
    void controllerChanged();

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};
} // namespace paddock
