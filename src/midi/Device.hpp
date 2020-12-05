#pragma once

#include "enums.hpp"

#include "utils/Expected.hpp"

#include <cstddef>
#include <memory>
#include <span>

namespace paddock
{
namespace midi
{
class AbstractDevice;

// Hardware MIDI device port for raw IO
class Device
{
public:
    template <typename T>
    class Model;

    enum class Type
    {
        Hardware,
        Software
    };

    struct Parameters
    {
        // Minimum available bytes to store in internal buffering before
        // the in poll handle is signalled.
        size_t minAvailableBytes;
    };

    static Expected<Device> open(const char* device, PortDirection direction);

    ~Device();

    Device(Device&& other);
    Device& operator=(Device&& other);

    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

    // Write the given buffer to the MIDI device
    // @return the number of bytes written or a system error.
    Expected<size_t> write(std::span<const std::byte> buffer);
    Expected<size_t> write(const std::vector<std::byte>& buffer);

    // Try to read at most as many bytes as the size of the input span.
    // @return the number of bytes read or a system error.
    Expected<size_t> read(std::span<std::byte> buffer);

    bool hasAvailableInput() const;

    std::error_code setParameters(const Parameters& parameters);

    // Get a poll handle for in/out events.
    // The implementations must guarantee that the returned pointers
    // are always the same (to allow comparison).
    std::shared_ptr<void> pollHandle(PollEvents events) const;

private:
    std::unique_ptr<AbstractDevice> _impl;

    template <typename T>
    Device(Model<T> impl);
};

} // namespace midi
} // namespace paddock
