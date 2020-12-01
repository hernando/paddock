#include "RawMidi.hpp"

#include "core/errors.hpp"
#include "midi/errors.hpp"

namespace paddock
{
namespace midi
{
namespace alsa
{
namespace
{
class RawMidiErrorCategory : public std::error_category
{
    const char* name() const noexcept override
    {
        return "paddock-alsa-rawmidi-error";
    }
    std::string message(int code) const override
    {
        using Error = RawMidi::Error;
        switch (static_cast<Error>(code))
        {
        case Error::openDeviceFailed:
            return "Could not open raw MIDI device";
        case Error::writeError:
            return "Error writing to MIDI device";
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

const RawMidiErrorCategory rawMidiErrorCategory{};

} // namespace

std::error_code make_error_code(RawMidi::Error error)
{
    return std::error_code{static_cast<int>(error), rawMidiErrorCategory};
}

Expected<RawMidi> RawMidi::open(const char* device, PortDirection direction)
{
    snd_rawmidi_t* midiIn = nullptr;
    snd_rawmidi_t* midiOut = nullptr;

    const bool read =
        direction == PortDirection::duplex || direction == PortDirection::read;
    const bool write =
        direction == PortDirection::duplex || direction == PortDirection::write;

    if (snd_rawmidi_open(read ? &midiIn : nullptr, write ? &midiOut : nullptr,
                         device, 0) == -1)
    {
        return tl::make_unexpected(Error::openDeviceFailed);
    }

    return RawMidi{Handle{midiIn, snd_rawmidi_close},
                   Handle{midiOut, snd_rawmidi_close}};
}

RawMidi::RawMidi(Handle inHandle, Handle outHandle)
    : _inHandle(std::move(inHandle))
    , _outHandle(std::move(outHandle))
{
}

RawMidi::~RawMidi() = default;

RawMidi::RawMidi(RawMidi&& other) = default;
RawMidi& RawMidi::operator=(RawMidi&& other) = default;

Expected<size_t> RawMidi::write(std::span<const std::byte> buffer)
{
    if (!_outHandle)
        return tl::make_unexpected(DeviceError::notWritable);

    size_t remaining = buffer.size();
    const std::byte* data = buffer.data();

    while (remaining)
    {
        if (ssize_t res = snd_rawmidi_write(_outHandle.get(), data, remaining))
        {
            if (res == -1)
                return tl::make_unexpected(Error::writeError);
            data += res;
            remaining -= res;
        }
    }
    if (snd_rawmidi_drain(_outHandle.get()) == -1)
        return tl::make_unexpected(Error::writeError);

    return buffer.size();
}

} // namespace alsa
} // namespace midi
} // namespace paddock
