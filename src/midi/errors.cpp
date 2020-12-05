#include "errors.hpp"

#include "core/errors.hpp"

namespace paddock
{
namespace midi
{
namespace
{
class DeviceErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "paddock-midi-device-error";
    }
    std::string message(int code) const override
    {
        using Error = DeviceError;
        switch (static_cast<Error>(code))
        {
        case Error::noImplementationAvailable:
            return "No raw MIDI device implementation available";
        case Error::notReadable:
            return "Raw MIDI device not readable";
        case Error::notWritable:
            return "Raw MIDI device not writable";
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

const DeviceErrorCategory deviceErrorCategory{};

class MidiEngineErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "paddock-midi-engine-error";
    }
    std::string message(int code) const override
    {
        using Error = EngineError;
        switch (static_cast<Error>(code))
        {
        case Error::noEngineAvailable:
            return "No MIDI engine available";
        case Error::noDeviceFound:
            return "No known hardware MIDI controller found";
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

const MidiEngineErrorCategory midiEngineErrorCategory{};

} // namespace

std::error_code make_error_code(DeviceError error)
{
    return std::error_code{static_cast<int>(error), deviceErrorCategory};
}

std::error_code make_error_code(EngineError error)
{
    return std::error_code{static_cast<int>(error), midiEngineErrorCategory};
}

} // namespace midi
} // namespace paddock
