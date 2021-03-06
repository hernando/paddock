#pragma once

#include "utils/Expected.hpp"

#include <functional>
#include <memory>

#include <system_error>

namespace paddock::core
{
class NsmSession
{
public:
    enum class Error
    {
        couldNotStartLoServer = 1,
        invalidServerAddress,
        failedToAnnounceToServer // Unused
    };

    struct Callbacks
    {
        // TODO
        std::function<void(bool)> activeCallback;

        // Parameters are path, session name and client id (mandatory for JACK)
        std::function<std::error_code(const std::string&, const std::string&,
                                      const std::string&)>
            openCallback;

        std::function<std::error_code()> saveCallback;
    };

    static Expected<NsmSession> startNsmSession(const char* nsmUrl,
                                                Callbacks callbacks);

    ~NsmSession();

    NsmSession(NsmSession&& other) noexcept;
    NsmSession& operator=(NsmSession&& other) noexcept;

    NsmSession(const NsmSession& other) = delete;
    NsmSession& operator=(const NsmSession& other) = delete;

    void setDirty(bool dirty);

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;

    NsmSession(std::unique_ptr<_Impl>);
};
} // namespace paddock::core

#include <system_error>

namespace std
{
template <>
struct is_error_code_enum<paddock::core::NsmSession::Error> : true_type
{
};
} // namespace std
