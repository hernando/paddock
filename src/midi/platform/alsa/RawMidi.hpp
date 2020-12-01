#pragma once

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
        writeError
    };

    static Expected<RawMidi> open(const char* device,
                                  PortDirection direction);

    ~RawMidi();

    RawMidi(RawMidi&& other);
    RawMidi& operator=(RawMidi&& other);

    RawMidi(const RawMidi& other) = delete;
    RawMidi& operator=(const RawMidi& other) = delete;

    Expected<size_t> write(std::span<const std::byte> buffer);

private:
    using Handle = std::unique_ptr<snd_rawmidi_t, int (*)(snd_rawmidi_t*)>;

    Handle _inHandle;
    Handle _outHandle;

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
