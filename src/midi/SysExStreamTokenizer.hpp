#pragma once

#include "Device.hpp"
#include "sysex.hpp"

#include "utils/byte.hpp"

#include <vector>

namespace paddock
{
namespace midi
{

template <size_t maxMessageSize>
class SysExStreamTokenizer
{
public:
    SysExStreamTokenizer()
        : _receiveBuffer{maxMessageSize}
    {
        _currentMessage.reserve(maxMessageSize);
    }

    void reset(Device* device)
    {
        _device = device;
        _readErrorState = false;
    }

    template <typename Decoder, typename ErrorCallback>
    void processInput(Decoder&& decoder, ErrorCallback&& errorCallback)
    {
        do
        {
            auto result = _device->read(_receiveBuffer);
            if (!result)
            {
                _setReadErrorState(errorCallback);
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
                        _setReadErrorState(errorCallback);
                        continue;
                    }
                    _currentMessage.push_back(byte);
                    continue;
                }

                auto payload =
                    std::span<const std::byte>{_currentMessage.begin() + 1,
                                               _currentMessage.end()};
                decoder(payload);
                _currentMessage.clear();
            }
        } while (_device->hasAvailableInput());

        return;
    }

private:
    Device* _device{nullptr};

    std::vector<std::byte> _receiveBuffer;
    std::vector<std::byte> _currentMessage;
    bool _readErrorState{false};

    template <typename ErrorCallback>
    void _setReadErrorState(const ErrorCallback& error)
    {
        _currentMessage.clear();
        _readErrorState = true;
        error();
    }
};

}
}
