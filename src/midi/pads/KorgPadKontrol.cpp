#include "KorgPadKontrol.hpp"

#include "korgPadKontrol/Program.hpp"
#include "korgPadKontrol/Scene.hpp"
#include "korgPadKontrol/nativeEvents.hpp"
#include "korgPadKontrol/sysex.hpp"

#include "midi/Client.hpp"
#include "midi/Device.hpp"
#include "midi/Engine.hpp"
#include "midi/SysExStreamTokenizer.hpp"
#include "midi/errors.hpp"

#include "core/Log.hpp"
#include "core/errors.hpp"

#include "utils/Expected.hpp"
#include "utils/overloaded.hpp"

#include <cassert>
#include <mutex>

namespace paddock
{
namespace midi
{
namespace sysex
{
using namespace korgPadKontrol::sysex;
}

using korgPadKontrol::Command;
using korgPadKontrol::Event;
using korgPadKontrol::Scene;
using namespace korgPadKontrol::events;

namespace
{
class PadKontrolErrorCategory : public std::error_category
{
    const char* name() const noexcept override { return "alsa-seq-error"; }
    std::string message(int code) const override
    {
        using Error = KorgPadKontrol::Error;
        switch (static_cast<Error>(code))
        {
        case Error::unrecognizedDevice:
            return "Device not recognized as KORG PadKontrol";
        case Error::packetCommunicationError:
            return "Packet Communication Command Error";
        default:
            throw std::logic_error("Unknown error code");
        }
    }
    bool equivalent(int code, const std::error_condition& condition) const
        noexcept override
    {
        return (condition == core::ErrorType::midi);
    }
};

const PadKontrolErrorCategory padKontrolErrorCategory{};

struct GlobalDataDumpRequest
{
    static constexpr auto& hostMessage = sysex::globalDataDumpReq;

    using Reply = Expected<std::vector<std::byte>>;
    std::promise<Reply> promise;

    bool handle(std::span<const std::byte> payload)
    {
        if (payload[4] != sysex::DATA_DUMP &&
            payload[6] != sysex::GLOBAL_DATA_DUMP)
        {
            return false;
        }
        promise.set_value(
            std::vector<std::byte>(payload.begin(), payload.end()));
        return true;
    }
};

struct CurrentSceneDataDumpRequest
{
    static constexpr auto& hostMessage = sysex::currentSceneDataDumpReq;

    using Reply = Expected<Scene>;
    std::promise<Reply> promise;

    bool handle(std::span<const std::byte> payload)
    {
        if (payload[4] != sysex::DATA_DUMP && payload[5] != 0x7F_b &&
            payload[6] != 0x02_b && payload[7] != 0x0A_b &&
            payload[8] != 0x02_b && payload[9] != sysex::CURRENT_SCENE_DUMP)
        {
            return false;
        }
        promise.set_value(
            korgPadKontrol::decodeScene({payload.begin() + 10, payload.end()}));
        return true;
    }
};

struct IdentityRequest
{
    static constexpr auto& hostMessage = sysex::inquiryMessageRequest;

    using Reply = Expected<std::vector<std::byte>>;
    std::promise<Reply> promise;

    bool handle(std::span<const std::byte> payload)
    {
        if (payload.size() != 13 || payload[0] != sysex::NON_REALTIME_MESSAGE ||
            payload[2] != sysex::GENERAL_INFORMATION ||
            payload[3] != sysex::IDENTITY)
        {
            return false;
        }
        promise.set_value(
            std::vector<std::byte>(payload.begin(), payload.end()));
        return true;
    }
};

template <bool on>
struct NativeModeRequest
{
    static constexpr auto& hostMessage =
        on ? sysex::nativeModeOnReq : sysex::nativeModeOffReq;

    using Reply = Expected<bool>;
    std::promise<Reply> promise;

    bool handle(std::span<const std::byte> payload)
    {
        // The structure of the reply is
        // [0xF0], 0x42, 0x4g, 0x6E, 0x08, PACKET_COMM, type, reply, [0xF7]
        if (payload.size() != 7 || payload[4] != sysex::NATIVE_MODE)
        {
            return false;
        }
        // 2 = Out, 3 = In
        promise.set_value(payload[6] == (on ? 0x03_b : 0x02_b));
        return true;
    }
};

struct PacketCommunicationCmd
{
    std::vector<std::byte> hostMessage;

    using Reply = Expected<bool>;
    std::promise<Reply> promise;

    explicit PacketCommunicationCmd(std::span<const std::byte> message)
        : hostMessage(message.begin(), message.end())
    {
    }

    bool handle(std::span<const std::byte> payload)
    {
        // The structure of the reply is
        // [0xF0], 0x42, 0x4g, 0x6E, 0x08, PACKET_COMM, type, reply, [0xF7]
        if (payload.size() != 7 || payload[4] != sysex::PACKET_COMM ||
            payload[5] != hostMessage[7])
        {
            return false;
        }
        promise.set_value(payload[6] == 0x00_b); // 0 = OK
        return true;
    }
};

struct ResetDefaultScene
{
    static constexpr auto& hostMessage = sysex::resetDefaultScene;

    using Reply = Expected<bool>;
    std::promise<Reply> promise;

    bool handle(std::span<const std::byte> payload)
    {
        // The structure of the reply is
        // [0xF0], 0x42, 0x4g, 0x6E, 0x08, PACKET_COMM, type, reply, [0xF7]
        if (payload.size() != 7 || payload[4] != sysex::PACKET_COMM ||
            (payload[5] != sysex::DATA_LOAD_COMPLETED &&
             payload[5] != sysex::DATA_LOAD_ERROR))
        {
            return false;
        }
        promise.set_value(payload[5] == sysex::DATA_LOAD_COMPLETED);
        return true;
    }
};

using CommandReply =
    std::variant<CurrentSceneDataDumpRequest, GlobalDataDumpRequest,
                 IdentityRequest, NativeModeRequest<true>,
                 NativeModeRequest<false>, PacketCommunicationCmd,
                 ResetDefaultScene>;

std::error_code check(std::future<Expected<bool>>&& future)
{
    auto result = future.get();
    if (!result)
        return result.error();
    if (!*result)
        return KorgPadKontrol::Error::packetCommunicationError;
    return std::error_code{};
}
#define POST_AND_CHECK(cmd)                    \
    if (auto error = check(_postCommand(cmd))) \
        return error;

} // namespace

std::error_code make_error_code(KorgPadKontrol::Error error)
{
    return std::error_code{static_cast<int>(error), padKontrolErrorCategory};
}

bool KorgPadKontrol::matches(const ClientInfo& clientInfo)
{
    return (clientInfo.name == "padKONTROL" &&
            clientInfo.type == ClientType::system &&
            clientInfo.outputs.size() == 3 && clientInfo.inputs.size() == 2);
}

class KorgPadKontrol::_Impl
{
public:
    _Impl(Engine* engine, ClientInfo&& deviceInfo, std::string&& midiClientName)
        : _engine{engine}
        , _deviceInfo{std::move(deviceInfo)}
        , _midiClientName{std::move(midiClientName)}
        , _mode{Mode::native}
    {
    }

    ~_Impl() { _stopPolling(); }

    Mode mode() const { return _mode; }

    std::error_code setMode(Mode mode)
    {
        if (_mode == mode && _client)
            return std::error_code{};

        _stopPolling();

        // The connections must be closed before proceeding.
        _device = std::nullopt;
        _client = std::nullopt;

        auto device = Device::open(_deviceInfo.inputs[1].hwDeviceId.c_str(),
                                   mode == Mode::native ? PortDirection::duplex
                                                        : PortDirection::write);

        if (!device)
            return device.error();

        auto client =
            _engine->open(_midiClientName, mode == Mode::native
                                               ? PortDirection::read
                                               : PortDirection::duplex);
        if (!client)
        {
            return client.error();
        }

        if (mode == Mode::normal)
        {
            if (auto error = client->connectInput(_deviceInfo, 1);
                error != std::error_code{})
            {
                return error;
            }
        }

        _device = std::move(*device);
        _client = std::move(*client);

        _startPolling();

        _mode = mode;

        if (mode == Mode::native)
        {
            POST_AND_CHECK(NativeModeRequest<false>{});
            POST_AND_CHECK(NativeModeRequest<true>{});
            POST_AND_CHECK(PacketCommunicationCmd{sysex::nativeEnableOutput});
            // Until this message is sent, the pad won't send any output.
            POST_AND_CHECK(PacketCommunicationCmd{
                sysex::packetCommunicationType2({}, "PAD")});
        }
        else
        {
            POST_AND_CHECK(NativeModeRequest<false>{});
            POST_AND_CHECK(ResetDefaultScene{});
        }

        return _handShake();
    }

    void setProgram(korgPadKontrol::Program program)
    {
        std::lock_guard<std::mutex> lock(_programMutex);
        _program = std::move(program);
    }

    std::future<Expected<bool>> sendNativeCommand(
        const korgPadKontrol::Command& command)
    {
        using korgPadKontrol::LedName;
        using korgPadKontrol::LedStatus;
        using namespace korgPadKontrol::events;

        const auto fireAndForget = [this](std::span<const std::byte> message) {
            std::promise<Expected<bool>> promise;
            auto result = _device->write(message);
            if (!result)
                promise.set_value(tl::unexpected(result.error()));
            else
                promise.set_value(true);
            return promise.get_future();
        };

        return std::visit(
            overloaded{[&](const LedOnCommand& command) {
                           return fireAndForget(sysex::displayLedCommand(
                               command.led, LedStatus::on, 0));
                       },
                       [&](const LedOffCommand& command) {
                           return fireAndForget(sysex::displayLedCommand(
                               command.led, LedStatus::off, 0));
                       },
                       [&](const BlinkLedCommand& command) {
                           return fireAndForget(sysex::displayLedCommand(
                               command.led, LedStatus::blink, 0));
                       },
                       [&](const SetAllLedCommand& command) {
                           return fireAndForget(sysex::packetCommunicationType2(
                               command.on, command.text));
                       },
                       [&](const TriggerLedCommand& command) {
                           return fireAndForget(sysex::displayLedCommand(
                               command.led, LedStatus::oneShot,
                               command.milliseconds / 9));
                       }},
            command);
    }

    Expected<korgPadKontrol::Scene> queryCurrentScene()
    {
        auto data = _postCommand(GlobalDataDumpRequest{}).get();
        // std::cout << "Decoded" << std::endl;
        // int i = 0;
        // for (auto byte : std::span{*data}.subspan(9))
        // {
        //    printf("%.2X ", int(byte));
        //    if (++i == 10)
        //        printf(" ");
        //    if (i == 20)
        //    {
        //        printf("\n");
        //        i = 0;
        //    }
        // }
        // printf("\n");
        return _postCommand(CurrentSceneDataDumpRequest{}).get();
    }

private:
    Engine* _engine{nullptr};
    ClientInfo _deviceInfo;
    std::string _midiClientName;
    Mode _mode;

    std::optional<Client> _client;
    std::optional<Device> _device;

    SysExStreamTokenizer<sysex::maxMessageSize> _tokenizer;

    std::mutex _pendingReplyMutex;
    std::vector<CommandReply> _pendingReplies;

    std::mutex _programMutex;
    korgPadKontrol::Program _program;

    void _processDeviceEvents()
    {
        assert(_mode == Mode::native);
        _tokenizer.processInput(
            [this](std::span<const std::byte> payload) {
                _decodeMessage(payload);
            },
            [this]() { _cancelPendingCommands(); });
    }

    void _decodeMessage(std::span<const std::byte> payload)
    {
        auto event = korgPadKontrol::decodeEvent(payload);
        if (event)
        {
            std::lock_guard<std::mutex> lock(_programMutex);
            _program.processEvent(*event, *_client, *_device);
        }

        std::lock_guard<std::mutex> lock(_pendingReplyMutex);
        _pendingReplies.erase(
            std::remove_if(_pendingReplies.begin(), _pendingReplies.end(),
                           [payload](CommandReply& reply) {
                               return std::visit(
                                   [payload](auto&& reply) {
                                       return reply.handle(payload);
                                   },
                                   reply);
                           }),
            _pendingReplies.end());
    }

    void _processClientEvents()
    {
        while (_client->hasEvents())
        {
            const auto event = _client->readEvent();
            if (!event)
            {
                core::log() << event.error().message();
                return;
            }

            std::visit( //
                overloaded{[this](auto&& event) {
                               _program.processEvent(event, *_client);
                           },
                           [this](const events::SysEx& event) {
                               _decodeMessage(std::span<const std::byte>{
                                   event.data.begin() + 1,
                                   event.data.end() - 1});
                           }},
                *event);
        }
    }

    void _startPolling()
    {
        if (_device)
        {
            _tokenizer.reset(&*_device);

            auto callback = [this](const void*, int) {
                _processDeviceEvents();
            };
            if (auto handle = _device->pollHandle(PollEvents::in))
            {
                _engine->add(handle, callback);
            }
        }
        if (_client)
        {
            auto callback = [this](const void*, int) {
                _processClientEvents();
            };
            if (auto handle = _client->pollHandle(PollEvents::in))
            {
                _engine->add(handle, callback);
            }
        }
    }

    void _stopPolling()
    {
        if (_device)
        {
            _engine->remove(_device->pollHandle(PollEvents::in)).wait();
        }
        if (_client)
        {
            _engine->remove(_client->pollHandle(PollEvents::in)).wait();
        }
    }

    std::error_code _handShake()
    {
        auto reply = _postCommand(IdentityRequest{});

        auto message = reply.get();
        if (!message)
            return message.error();

        if ((*message)[4] != sysex::KORG || (*message)[5] != sysex::SW_PROJECT)
            return Error::unrecognizedDevice;

        return std::error_code{};
    }

    template <typename T>
    std::future<typename T::Reply> _postCommand(T&& inCommand)
    {
        using Command = std::decay_t<decltype(inCommand)>;

        Command& command = [this](auto&& command) -> Command& {
            std::lock_guard<std::mutex> lock(_pendingReplyMutex);
            _pendingReplies.push_back(std::forward<Command>(command));
            return std::get<Command>(_pendingReplies.back());
        }(std::forward<T>(inCommand));

        auto future = command.promise.get_future();

        if (auto result = _device->write(command.hostMessage); !result)
        {
            command.promise.set_value(tl::unexpected(result.error()));

            std::lock_guard<std::mutex> lock(_pendingReplyMutex);
            _pendingReplies.pop_back();
        }

        return future;
    }

    void _cancelPendingCommands()
    {
        std::lock_guard<std::mutex> lock(_pendingReplyMutex);
        // Cancel all pending commands
        for (auto& command : _pendingReplies)
        {
            std::visit(
                [](auto&& command) {
                    command.promise.set_value(
                        tl::make_unexpected(DeviceError::streamReadError));
                },
                command);
        }
        _pendingReplies.clear();
    }
}; // namespace paddock

Expected<KorgPadKontrol> KorgPadKontrol::open(Engine* engine,
                                              ClientInfo deviceInfo,
                                              std::string midiClientName)
{
    auto impl = std::make_unique<_Impl>(engine, std::move(deviceInfo),
                                        std::move(midiClientName));
    if (auto error = impl->setMode(Mode::normal); error != std::error_code{})
        return tl::unexpected(error);

    return KorgPadKontrol(std::move(impl));
}

KorgPadKontrol::KorgPadKontrol(std::unique_ptr<_Impl> impl)
    : _impl(std::move(impl))
{
}

KorgPadKontrol::~KorgPadKontrol() = default;

KorgPadKontrol::KorgPadKontrol(KorgPadKontrol&& other) noexcept = default;
KorgPadKontrol& KorgPadKontrol::operator=(KorgPadKontrol&& other) noexcept =
    default;

KorgPadKontrol::Mode KorgPadKontrol::mode() const
{
    return _impl->mode();
}

std::error_code KorgPadKontrol::setMode(Mode mode)
{
    return _impl->setMode(mode);
}

void KorgPadKontrol::setProgram(korgPadKontrol::Program program)
{
    _impl->setProgram(std::move(program));
}

std::future<Expected<bool>> KorgPadKontrol::sendNativeCommand(
    const korgPadKontrol::Command& event)
{
    return _impl->sendNativeCommand(event);
}

Expected<korgPadKontrol::Scene> KorgPadKontrol::queryCurrentScene()
{
    return _impl->queryCurrentScene();
}

} // namespace midi
} // namespace paddock
