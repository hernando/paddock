#include "Session.hpp"

#include "utils.hpp"

#include "paddock/defines.h"

#include "core/Log.hpp"
#include "core/Program.hpp"

#ifdef PADDOCK_USE_LIBLO
#include "core/NsmSession.hpp"
#endif

#include <iostream>
#include <optional>

namespace paddock
{
#ifndef PADDOCK_USE_LIBLO
namespace core
{
class NsmSession
{
    class Callbacks
    {
        void* activeCallback;
        void* openCallback;
        void* saveCallback;
    };

    void setDirty(bool) {}
};
} // namespace core
}
std::optional<core::NsmSession> tryStartNsmSession(NsmSession::Callbacks)
{
    return std::nullopt;
}
#else
std::optional<core::NsmSession> tryStartNsmSession(
    core::NsmSession::Callbacks callbacks)
{
    auto nsmUrl = getenv("NSM_URL");
    if (nsmUrl == nullptr)
        return std::nullopt;

    auto session =
        core::NsmSession::startNsmSession(nsmUrl, std::move(callbacks));
    if (!session)
    {
        // TODO print error
        return std::nullopt;
    }
    return std::move(*session);
}
#endif

class Session::_Impl
{
public:
    _Impl(Session* parent)
        : _parent(parent)
    {
        // Don't rely on default intialization, otherwise if this member
        // variable is not declared last the open notification could be
        // accessing a half contructed object.
        _nsmSession = tryStartNsmSession(core::NsmSession::Callbacks{
            [](bool) {},
            [this](const auto&... args) { return this->open(args...); },
            [this]() { return this->save(); }});
    }

    bool hasSession() const { return _nsmSession.has_value(); }

    std::error_code open(const std::string& projectPath,
                         const std::string& displayName,
                         const std::string& clientId)
    {
        return openProgram(projectPath + "/program.pad");
    }

    std::error_code save()
    {
        core::log() << "SAVE";
        return std::error_code(); // success
    }

    void sendDirty(bool dirty)
    {
        if (_nsmSession)
            _nsmSession->setDirty(dirty);
    }

    std::error_code openProgram(std::string filePath)
    {
        auto program =
            execInMainThread([this] { return new core::Program(_parent); });

        if (!program)
            return program.error();

        _filePath = std::move(filePath);

        if (_program)
            _program->deleteLater();

        _program = *program;

        connect(_program, &core::Program::dirtyChanged,
                [this]() { this->sendDirty(_program->dirty()); });

        Q_EMIT _parent->programChanged();
        return std::error_code(); // success
    }

    core::Program* program() const { return _program; }

private:
    Session* _parent;

    core::Program* _program{nullptr};
    std::string _filePath;
    std::optional<core::NsmSession> _nsmSession;
};

Session::Session()
    : _impl{std::make_unique<_Impl>(this)}
{
}

Session::~Session() = default;

core::Program* Session::program() const
{
    return _impl->program();
}

bool Session::isNsmSession() const
{
    return _impl->hasSession();
}

std::error_code Session::openProgram(const std::string& filePath)
{
    return _impl->openProgram(filePath);
}

} // namespace paddock