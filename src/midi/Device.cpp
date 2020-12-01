#include "Device.hpp"

#include "platform/alsa/RawMidi.hpp"

namespace paddock
{
namespace midi
{
class AbstractDevice
{
public:
    virtual ~AbstractDevice() {}
    virtual Expected<size_t> write(std::span<const std::byte> buffer) = 0;
};

template <typename T>
class Device::Model : public AbstractDevice
{
public:
    Model(T device)
        : _device(std::move(device))
    {
    }

    Expected<size_t> write(std::span<const std::byte> buffer) final
    {
        return _device.write(buffer);
    }

private:
    T _device;
};

Expected<Device> Device::open(const char* name, PortDirection direction)
{
#if PADDOCK_USE_ALSA
    return alsa::RawMidi::open(name, direction)
        .and_then([](alsa::RawMidi device) -> Expected<Device> {
            return Device{Model{std::move(device)}};
        });
#else
    return make_error_code(Error::NoImplementationAvailable);
#endif
}

template <typename T>
Device::Device(Model<T> impl)
    : _impl(new Model<T>(std::move(impl)))
{
}

Device::~Device() = default;

Device::Device(Device&&) = default;
Device& Device::operator=(Device&& other) = default;

Expected<size_t> Device::write(std::span<const std::byte> buffer)
{
    return _impl->write(buffer);
}

Expected<size_t> Device::write(const std::vector<std::byte>& buffer)
{
    return _impl->write(buffer);
}

} // namespace midi
} // namespace paddock
