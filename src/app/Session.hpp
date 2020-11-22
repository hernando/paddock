#pragma once

#include <QObject>

#include <memory>
#include <system_error>

namespace paddock
{
namespace core
{
class Program;
}

class Session : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        paddock::core::Program* program READ program NOTIFY programChanged)

    Q_PROPERTY(bool isNsmSession READ isNsmSession CONSTANT)

public:
    Session();
    ~Session();

    core::Program* program() const;

    bool isNsmSession() const;

    std::error_code openProgram(const std::string& filePath);

signals:
    void programChanged();

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};
} // namespace paddock
