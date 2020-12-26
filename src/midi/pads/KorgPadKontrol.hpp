#pragma once

#include "korgPadKontrol/nativeEvents.hpp"

#include <utils/Expected.hpp>

#include <future>
#include <memory>

namespace paddock
{
namespace midi
{
class ClientInfo;
class Engine;

namespace korgPadKontrol
{
class Program;
class Scene;
}

class KorgPadKontrol
{
public:
    friend class Engine;

    enum class Error
    {
        unrecognizedDevice = 1,
        packetCommunicationError
    };

    enum class Mode
    {
        native,
        normal
    };

    static bool matches(const ClientInfo& client);

    static Expected<KorgPadKontrol> open(Engine* engine, ClientInfo device,
                                         std::string midiClientName);

    ~KorgPadKontrol();

    KorgPadKontrol(KorgPadKontrol&& other) noexcept;
    KorgPadKontrol& operator=(KorgPadKontrol&& other) noexcept;

    KorgPadKontrol(const KorgPadKontrol& other) = delete;
    KorgPadKontrol& operator=(const KorgPadKontrol& other) = delete;

    ClientId deviceId() const;

    std::error_code setMode(Mode mode);
    Mode mode() const;

    std::error_code setProgram(korgPadKontrol::Program program);

    // We need future.then to return std::future<std::error_code> without
    // complicatint the implementation.
    std::future<Expected<bool>> sendNativeCommand(
        const korgPadKontrol::Command& command);

    Expected<korgPadKontrol::Scene> queryCurrentScene();

private:
    class _Impl;
    // A shared ptr is needed to capture it in the callbacks passed
    // to the poller.
    std::unique_ptr<_Impl> _impl;

    KorgPadKontrol(std::unique_ptr<_Impl> _impl);

    void _stopPolling();
    void _deviceInEvent();
    void _clientInEvent();
};

} // namespace midi
} // namespace paddock

#include <system_error>

namespace std
{
template <>
struct is_error_code_enum<paddock::midi::KorgPadKontrol::Error> : true_type
{
};
} // namespace std
