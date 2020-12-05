#include "KorgPadKontrol.hpp"

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
#include <iostream>

#define ECHO

namespace paddock
{
namespace midi
{
namespace sysex
{
using namespace korgPadKontrol::sysex;
}

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

using RequestResult = Expected<std::vector<std::byte>>;

struct IdentityRequest
{
    std::promise<RequestResult> promise;

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
        {
            return device.error();
        }
        device->write(sysex::nativeModeOff);
        if (mode == Mode::native)
        {
            device->write(sysex::nativeModeOn);
            device->write(sysex::nativeModeInit);
            device->write(sysex::nativeEnableOutput);
            device->write(sysex::nativeModeTest);
        }

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

        return _handShake();
    }

private:
    Engine* _engine{nullptr};
    ClientInfo _deviceInfo;
    std::string _midiClientName;
    Mode _mode;

    std::optional<Client> _client;
    std::optional<Device> _device;

    SysExStreamTokenizer<sysex::maxMessageSize> _tokenizer;

    std::vector<PendingRequest> _pendingRequests;

    void _deviceInEvent()
    {
        assert(_mode == Mode::native);
        _tokenizer.processInput(
            [this](std::span<const std::byte> payload) {
                _decodeMessage(payload);
            },
            [this]() { _cancelPendingRequests(); });
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

            std::visit(overloaded{[](auto&& event) {
                                      std::cout << event.description
                                                << std::endl;
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
        if (auto result = _device->write(sysex::inquiryMessageRequest); !result)
        {
            return result.error();
        }

        auto message = reply.get();
        if (!message)
            return message.error();

        if ((*message)[4] != sysex::KORG || (*message)[5] != sysex::SW_PROJECT)
            return Error::unrecognizedDevice;

        return std::error_code{};
    }

    template <typename Request>
    std::future<RequestResult> _postRequest(Request&& request)
    {
        _pendingRequests.emplace_back(std::forward<Request>(request));
        return std::visit(
            [](auto&& request) { return request.promise.get_future(); },
            _pendingRequests.back());
    }

    void _cancelPendingRequests()
    {
        // Cancel all pending requests
        for (auto& request : _pendingRequests)
        {
            std::visit(
                [](auto&& request) {
                    request.promise.set_value(
                        tl::make_unexpected(DeviceError::streamReadError));
                },
                request);
        }
        _pendingRequests.clear();
    }
};

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

std::error_code KorgPadKontrol::setMode(Mode mode)
{
    return _impl->setMode(mode);
}

KorgPadKontrol::Mode KorgPadKontrol::mode() const
{
    return _impl->mode();
}

} // namespace midi
} // namespace paddock
