#pragma once

#include "midi/Device.hpp"
#include "midi/enums.hpp"

#include "utils/Expected.hpp"

#include <alsa/asoundlib.h>

#include <memory>
#include <span>

namespace paddock
{
namespace midi
{
namespace alsa
{
class RawMidi
{
public:
    enum class Error
    {
        openDeviceFailed = 1,
        setParametersFailed,
        readError,
        writeError
    };

    static Expected<RawMidi> open(const char* device, PortDirection direction);

    ~RawMidi();

    RawMidi(RawMidi&& other);
    RawMidi& operator=(RawMidi&& other);

    RawMidi(const RawMidi& other) = delete;
    RawMidi& operator=(const RawMidi& other) = delete;

    Expected<size_t> write(std::span<const std::byte> buffer,
                           bool flush = false);
    Expected<size_t> read(std::span<std::byte> buffer);

    bool hasAvailableInput() const;

    std::error_code setParameters(const Device::Parameters& parameters);

    std::shared_ptr<void> pollHandle(PollEvents events) const;

private:
    using Handle = std::unique_ptr<snd_rawmidi_t, int (*)(snd_rawmidi_t*)>;

    Handle _inHandle;
    Handle _outHandle;

    std::shared_ptr<void> _inPollHandle;
    std::shared_ptr<void> _outPollHandle;

    mutable size_t _availableBytes{0};

    RawMidi(Handle inHandle, Handle outHandle);
};

} // namespace alsa
} // namespace midi
} // namespace paddock

#include <system_error>

namespace std
{
template <>
struct is_error_code_enum<paddock::midi::alsa::RawMidi::Error> : true_type
{
};
} // namespace std
