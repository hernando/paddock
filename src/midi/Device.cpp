#include "Device.hpp"

namespace paddock
{
namespace midi
{
class Device::_Impl
{
public:
    _Impl() {}
};

Device::Device()
    : _impl(std::make_unique<_Impl>())
{
}

Device::~Device() = default;

Device::Device(Device&&) = default;
Device& Device::operator=(Device&& other) = default;

} // namespace midi
} // namespace paddock
