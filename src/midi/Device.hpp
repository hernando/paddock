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

    static Expected<Device> open(const char* device, PortDirection direction);

    ~Device();

    // Write the given buffer to the MIDI device
    // @return the number of bytes written or a system error.
    Expected<size_t> write(std::span<const std::byte> buffer);
    Expected<size_t> write(const std::vector<std::byte>& buffer);

    Device(Device&& other);
    Device& operator=(Device&& other);

    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

private:
    std::unique_ptr<AbstractDevice> _impl;

    template <typename T>
    Device(Model<T> impl);
};

} // namespace midi
} // namespace paddock
