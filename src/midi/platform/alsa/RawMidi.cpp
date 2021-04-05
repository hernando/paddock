#include "RawMidi.hpp"

#include "core/errors.hpp"
#include "midi/errors.hpp"

namespace paddock::midi::alsa
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
        case Error::deviceBusy:
            return "MIDI device busy";
        case Error::openDeviceFailed:
            return "Could not open raw MIDI device";
        case Error::setParametersFailed:
            return "Error setting raw MIDI device parameters";
        case Error::readError:
            return "Error reading from MIDI device";
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

std::shared_ptr<void> getPollDescriptor(snd_rawmidi_t* device)
{
    if (device == nullptr)
        return {};

    // This function has return 1 in the alsa-lib implementation for
    // years. So let's assume that's the case, but throw if it's not.
    if (snd_rawmidi_poll_descriptors_count(device) != 1)
        throw std::logic_error("Poll descriptors is not 1");

    auto fd = std::make_shared<pollfd>();
    // No need to check the return value, it must be one.
    snd_rawmidi_poll_descriptors(device, fd.get(), 1);
    return std::static_pointer_cast<void>(fd);
}

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

    // Trying to open first in non-blocking mode. Otherwise the application
    // will get blocked forever if another process has the device open.
    if (int error = snd_rawmidi_open(read ? &midiIn : nullptr,
                                     write ? &midiOut : nullptr, device,
                                     SND_RAWMIDI_NONBLOCK);
        error < 0)
    {
        if (error == -EAGAIN || error == -EBUSY)
            return tl::make_unexpected(Error::deviceBusy);
        return tl::make_unexpected(Error::openDeviceFailed);
    }

    // Closing to reopen in synchronous mode.
    if (read)
        snd_rawmidi_close(midiIn);
    if (write)
        snd_rawmidi_close(midiOut);

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
    , _inPollHandle(getPollDescriptor(_inHandle.get()))
    , _outPollHandle(getPollDescriptor(_outHandle.get()))
{
}

RawMidi::~RawMidi() = default;

RawMidi::RawMidi(RawMidi&& other) = default;
RawMidi& RawMidi::operator=(RawMidi&& other) = default;

Expected<size_t> RawMidi::write(std::span<const std::byte> buffer, bool flush)
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
    if (flush)
    {
        if (snd_rawmidi_drain(_outHandle.get()) == -1)
            return tl::make_unexpected(Error::writeError);
    }

    return buffer.size();
}

Expected<size_t> RawMidi::read(std::span<std::byte> buffer)
{
    // I'm not sure about the performance of status query (it call ioctl)
    // However, using non blocking IO affect the writes as well and I'm not
    // sure of the side effects of opening 2 connections in different mode.
    size_t capacity = buffer.size();
    snd_rawmidi_status_t* status;
    snd_rawmidi_status_alloca(&status);
    if (snd_rawmidi_status(_inHandle.get(), status) == -1)
        return tl::make_unexpected(Error::readError);
    _availableBytes = snd_rawmidi_status_get_avail(status);
    const auto readBytes = std::min(capacity, _availableBytes);
    _availableBytes -= readBytes;
    if (snd_rawmidi_read(_inHandle.get(), buffer.data(), readBytes) == -1)
        return tl::make_unexpected(Error::readError);
    return readBytes;
}

bool RawMidi::hasAvailableInput() const
{
    if (_availableBytes)
        return true;

    snd_rawmidi_status_t* status;
    snd_rawmidi_status_alloca(&status);
    if (snd_rawmidi_status(_inHandle.get(), status) == -1)
        return false;
    _availableBytes = snd_rawmidi_status_get_avail(status);
    return _availableBytes != 0;
}

std::error_code RawMidi::setParameters(const Device::Parameters& parameters)
{
    snd_rawmidi_params_t* params;
    snd_rawmidi_params_alloca(&params);
    if (snd_rawmidi_params_set_avail_min(_inHandle.get(), params,
                                         parameters.minAvailableBytes) == -1)
        return Error::setParametersFailed;
    if (snd_rawmidi_params(_inHandle.get(), params) == -1)
        return Error::setParametersFailed;
    return std::error_code{};
}

std::shared_ptr<void> RawMidi::pollHandle(PollEvents events) const
{
    switch (events)
    {
    case PollEvents::in:
        return _inPollHandle;
    case PollEvents::out:
        return _outPollHandle;
    default:
        throw std::logic_error("invalid value");
    }
}

} // namespace paddock::midi::alsa
