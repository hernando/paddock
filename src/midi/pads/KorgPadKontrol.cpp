#include "KorgPadKontrol.hpp"

#include "midi/Client.hpp"
#include "midi/Device.hpp"
#include "midi/Engine.hpp"

#include "core/Log.hpp"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
using namespace std::literals::chrono_literals;

// #define ECHO

namespace paddock
{
namespace midi
{
namespace
{
constexpr std::byte operator"" _b(unsigned long long c)
{
    return std::byte{static_cast<unsigned char>(c)};
}

namespace sysex
{
constexpr size_t maxMessageSize = 128;

constexpr auto START = 0xF0_b;
constexpr auto END = 0xF7_b;
constexpr auto KORG = 0x42_b;
constexpr auto PADKONTROL = 0x08_b;

#define SYSEX_HEADER START, KORG, 0x40_b, 0x6E_b, PADKONTROL

// clang-format off

// Values for 6th bytes
// 6th Byte = cd : 0dvmmmmm  d (0: Host->Controller, 1: Controller->Host)
//                           v (0: 2 Bytes Data Format, 1: Variable)
//                           mmmmm (Command Number)
// * = only in native mode

// Host commands
constexpr auto NATIVE_MODE_REQ = 0x00_b; // 000 00000
constexpr auto DISPLAY_LED     = 0x01_b; // 000 00001 *
constexpr auto DISPLAY_LCD     = 0x22_b; // 001 00010 *
constexpr auto PORT_DETECT_REQ = 0x1E_b; // 000 11110
constexpr auto DATA_DUMP_REQ   = 0x1F_b; // 000 11111
constexpr auto PACKET_COMM_REQ = 0x3F_b; // 001 11111

// Controller replies
constexpr auto NATIVE_MODE     = 0x40_b; // 010 00000
constexpr auto ENCODER_OUTPUT  = 0x43_b; // 010 00011 *
constexpr auto PAD_OUTPUT      = 0x45_b; // 010 00101 *
constexpr auto PEDAL_OUTPUT    = 0x47_b; // 010 00111 *
constexpr auto SW_OUTPUT       = 0x48_b; // 010 01000 *
constexpr auto KNOB_OUTPUT     = 0x49_b; // 010 01001 *
constexpr auto XY_OUTPUT       = 0x4B_b; // 010 01011 *
constexpr auto PORT_DETECT     = 0x7E_b; // 011 11110
constexpr auto PACKET_COMM     = 0x5F_b; // 010 11111
constexpr auto DATA_DUMP       = 0x7F_b; // 011 11111

// Function codes for requests and replies
constexpr auto SCENE_CHANGE_REQ       = 0x14_b;
constexpr auto SCENE_CHANGE           = 0x4F_b;
constexpr auto CURRENT_SCENE_DUMP_REQ = 0x10_b;
constexpr auto CURRENT_SCENE_DUMP     = 0x40_b;
constexpr auto GLOBAL_DATA_DUMP_REQ   = 0x0E_b;
constexpr auto GLOBAL_DATA_DUMP       = 0x51_b;
constexpr auto SCENE_WRITE_REQ        = 0x11_b;
constexpr auto DATA_FORMAT_ERROR      = 0x26_b;
constexpr auto DATA_LOAD_COMPLETED    = 0x23_b;
constexpr auto DATA_LOAD_ERROR        = 0x24_b;
constexpr auto WRITE_COMPLETED        = 0x21_b;
constexpr auto WRITE_ERROR            = 0x22_b;
// Bytes 00-03 are reserved for Native KORG mode Dump Data

std::vector<std::byte> nativeModeOff = {
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x00_b, END};

std::vector<std::byte> nativeModeOn = {
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x01_b, END};

std::vector<std::byte> nativeModeInit = {
    SYSEX_HEADER,
    0x3F_b, 0x0A_b, 0x01_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x29_b, 0x29_b, 0x29_b, END};
std::vector<std::byte> nativeModeTest = {
    SYSEX_HEADER,
    DISPLAY_LCD, 0x04_b, 0x00_b, 0x59_b, 0x45_b, 0x53_b, END};

std::vector<std::byte> nativeEnableOutput = {
    SYSEX_HEADER,
    0x3F_b, 0x2A_b, 0x00_b, 0x00_b, 0x05_b, 0x05_b, 0x05_b,
    0x7F_b, 0x7E_b, 0x7F_b, 0x7F_b, 0x03_b, 0x0A_b, 0x0A_b,
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b, 0x07_b,
    0x08_b, 0x09_b, 0x0A_b, 0x0B_b, 0x0C_b, 0x0d_b, 0x0E_b,
    0x0F_b, 0x10_b, END};

std::vector<std::byte> inquiryMessageRequest = {
    START, 0x7E_b, 0x7F_b, 0x06_b, 0x01_b, END};
};
// clang-format on

struct IdentityRequest
{
    std::promise<std::vector<std::byte>> promise;

    bool handle(std::span<const std::byte> payload)
    {
        if (payload.size() == 13 && payload[0] == 0x7E_b &&
            payload[2] == 0x06_b && payload[3] == 0x02_b)
        {
            promise.set_value(
                std::vector<std::byte>(payload.begin(), payload.end()));
            return true;
        }
        return false;
    }
};

using PendingRequest = std::variant<IdentityRequest>;

} // namespace

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
        , _receiveBuffer{sysex::maxMessageSize}
    {
        _currentMessage.reserve(sysex::maxMessageSize);
    }

    ~_Impl() { _stopPolling(); }

    Mode mode() const { return _mode; }

    std::error_code setNativeMode()
    {
        if (_mode == Mode::native && _client)
            return std::error_code{};

        _stopPolling();

        // The connections must be closed before proceeding.
        _device = std::nullopt;
        _client = std::nullopt;

        auto device = Device::open(_deviceInfo.inputs[1].hwDeviceId.c_str(),
                                   PortDirection::duplex);
        if (!device)
        {
            return device.error();
        }
        device->write(sysex::nativeModeOff);
        device->write(sysex::nativeModeOn);
        device->write(sysex::nativeModeInit);
        device->write(sysex::nativeEnableOutput);
        device->write(sysex::nativeModeTest);

        auto client = _engine->open(_midiClientName, PortDirection::read);
        if (!client)
        {
            return client.error();
        }

        _device = std::move(*device);
        _client = std::move(*client);

        _startPolling();

        _mode = Mode::native;

        return _handShake();
    }

    std::error_code setNormalMode()
    {
        if (_mode == Mode::normal && _client)
            return std::error_code{};

        _stopPolling();

        // The connections must be closed before proceeding.
        _device = std::nullopt;
        _client = std::nullopt;

        auto device = Device::open(_deviceInfo.inputs[1].hwDeviceId.c_str(),
                                   PortDirection::write);
        if (!device)
        {
            return device.error();
        }
        device->write(sysex::nativeModeOff);

        auto client = _engine->open(_midiClientName, PortDirection::duplex);
        if (!client)
        {
            return client.error();
        }

        if (auto error = client->connectInput(_deviceInfo, 1);
            error != std::error_code{})
        {
            return error;
        }

        // TODO there's a race condition between this assignments
        // and the _clientInEvent and _deviceInEvent callbacks.
        _device = std::move(*device);
        _client = std::move(*client);

        _startPolling();

        _mode = Mode::normal;

        return _handShake();
    }

private:
    Engine* _engine{nullptr};
    ClientInfo _deviceInfo;
    std::string _midiClientName;
    Mode _mode;

    std::optional<Client> _client;
    std::optional<Device> _device;

    std::vector<std::byte> _receiveBuffer;
    std::vector<std::byte> _currentMessage;
    bool _readErrorState{false};

    std::vector<PendingRequest> _pendingRequests;

    void _deviceInEvent()
    {
        assert(_mode == Mode::native);

        do
        {
            auto result = _device->read(_receiveBuffer);
            if (!result)
            {
                _setReadErrorState();
                return;
            }

            auto bytesRead = *result;

            if (bytesRead == 0)
                return; // Nothing read?

            for (size_t i = 0; i != bytesRead; ++i)
            {
                const auto byte = _receiveBuffer[i];

                if (_readErrorState)
                {
                    // Skip util END is read to resync with the stream
                    if (byte != sysex::END)
                        continue;
                    _readErrorState = false;
                    continue;
                }

                if (byte != sysex::END)
                {
                    if (_currentMessage.empty() && byte != sysex::START)
                    {
                        // Garbage ?
                        _setReadErrorState();
                        continue;
                    }
                    _currentMessage.push_back(byte);
                    continue;
                }

                auto payload =
                    std::span<const std::byte>{_currentMessage.begin() + 1,
                                               _currentMessage.end()};
                _decodeMessage(payload);
                _currentMessage.clear();
            }
        } while (_device->hasAvailableInput());
    }

    void _setReadErrorState()
    {
        _currentMessage.clear();
        _readErrorState = true;
    }

    void _decodeMessage(std::span<const std::byte> payload)
    {
#ifdef ECHO
        std::cout << "Decoded" << std::endl;
        for (auto byte : payload)
        {
            printf("%.2X ", int(byte));
        }
        printf("\n");
#endif

        _pendingRequests.erase(
            std::remove_if(_pendingRequests.begin(), _pendingRequests.end(),
                           [payload](PendingRequest& request) {
                               return std::visit(
                                   [payload](auto&& request) {
                                       return request.handle(payload);
                                   },
                                   request);
                           }),
            _pendingRequests.end());
    }

    void _clientInEvent()
    {
        while (_client->hasEvents())
        {
            const auto event = _client->readEvent();
            if (!event)
            {
                return;
            }
            std::visit(
                [](auto&& event) {
                    std::cout << event.description << std::endl;
                },
                *event);
        }
    }

    void _startPolling()
    {
        if (_device)
        {
            auto callback = [this](const void*, int) { _deviceInEvent(); };
            if (auto handle = _device->pollHandle(PollEvents::in))
            {
                _engine->add(handle, callback);
            }
        }
        if (_client)
        {
            auto callback = [this](const void*, int) { _clientInEvent(); };
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
        auto reply = _postRequest(IdentityRequest{});
        _device->write(sysex::inquiryMessageRequest);

        std::cout << "Identity" << std::endl;
        auto message = reply.get();
        for (auto byte : message)
        {
            printf("%.2X ", int(byte));
        }
        printf("\n");

        return std::error_code{};
    }

    template <typename Request>
    std::future<std::vector<std::byte>> _postRequest(Request&& request)
    {
        _pendingRequests.emplace_back(std::forward<Request>(request));
        return std::visit(
            [](auto&& request) { return request.promise.get_future(); },
            _pendingRequests.back());
    }
};

Expected<KorgPadKontrol> KorgPadKontrol::open(Engine* engine,
                                              ClientInfo deviceInfo,
                                              std::string midiClientName)
{
    auto impl = std::make_unique<_Impl>(engine, std::move(deviceInfo),
                                        std::move(midiClientName));
    if (auto error = impl->setNativeMode(); error != std::error_code{})
        return tl::unexpected(error);

    return KorgPadKontrol(std::move(impl));
}

KorgPadKontrol::KorgPadKontrol(std::unique_ptr<_Impl> impl)
    : _impl(std::move(impl))
{
}

KorgPadKontrol::~KorgPadKontrol() = default;

KorgPadKontrol::KorgPadKontrol(KorgPadKontrol&& other) = default;
KorgPadKontrol& KorgPadKontrol::operator=(KorgPadKontrol&& other) = default;

std::error_code KorgPadKontrol::setMode(Mode mode)
{
    if (mode == Mode::native)
        return _impl->setNativeMode();
    else
        return _impl->setNormalMode();
}

KorgPadKontrol::Mode KorgPadKontrol::mode() const
{
    return _impl->mode();
}

} // namespace midi
} // namespace paddock
