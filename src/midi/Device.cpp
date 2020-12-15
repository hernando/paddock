#include "Device.hpp"

#include "midi/errors.hpp"
#include "platform/alsa/RawMidi.hpp"

namespace paddock
{
namespace midi
{
class AbstractDevice
{
public:
    virtual ~AbstractDevice() {}
    virtual Expected<size_t> write(std::span<const std::byte> buffer,
                                   bool flush) = 0;
    virtual Expected<size_t> read(std::span<std::byte> buffer) = 0;
    virtual bool hasAvailableInput() const = 0;
    virtual std::error_code setParameters(
        const Device::Parameters& parameters) = 0;
    virtual std::shared_ptr<void> pollHandle(PollEvents events) const = 0;
};

template <typename T>
class Device::Model : public AbstractDevice
{
public:
    Model(T device)
        : _device(std::move(device))
    {
    }

    Expected<size_t> write(std::span<const std::byte> buffer, bool flush) final
    {
        return _device.write(buffer);
    }

    Expected<size_t> read(std::span<std::byte> buffer) final
    {
        return _device.read(buffer);
    }

    bool hasAvailableInput() const final { return _device.hasAvailableInput(); }

    std::error_code setParameters(const Device::Parameters& parameters) final
    {
        return _device.setParameters(parameters);
    }

    std::shared_ptr<void> pollHandle(PollEvents events) const final
    {
        return _device.pollHandle(events);
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
    return tl::make_unexpected(DeviceError::noImplementationAvailable);
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

Expected<size_t> Device::write(std::span<const std::byte> buffer, bool flush)
{
    return _impl->write(buffer, flush);
}

Expected<size_t> Device::write(const std::vector<std::byte>& buffer, bool flush)
{
    return _impl->write(buffer, flush);
}

Expected<size_t> Device::read(std::span<std::byte> buffer)
{
    return _impl->read(buffer);
}

bool Device::hasAvailableInput() const
{
    return _impl->hasAvailableInput();
}

std::error_code Device::setParameters(const Parameters& parameters)
{
    return _impl->setParameters(parameters);
}

std::shared_ptr<void> Device::pollHandle(PollEvents events) const
{
    return _impl->pollHandle(events);
}

} // namespace midi
} // namespace paddock
