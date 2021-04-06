#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>

#include <memory>
#include <system_error>

namespace paddock
{
namespace midi
{
class Engine;
}

class Program;

class Session : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QVariant controller READ controller NOTIFY controllerChanged)
    Q_PROPERTY(
        paddock::Program* program READ program NOTIFY programChanged)

    Q_PROPERTY(bool isNsmSession READ isNsmSession CONSTANT)

public:
    Session();
    ~Session();

    std::error_code init();

    Program* program() const;

    QVariant controller();

    bool isNsmSession() const;

    /// Name to use for the JACK client
    const std::string& name() const;

    std::error_code open(const std::string& filePath);
    std::error_code save(const std::string& filePath) const;

public slots:
    void save(const QUrl& filePath) const;

signals:
    void programChanged();
    void controllerChanged();

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};
} // namespace paddock
