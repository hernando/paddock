#pragma once

#include "Events.hpp"

#include <memory>

namespace paddock
{
namespace midi
{
class Device
{
public:
    enum class Type
    {
        Hardware,
        Software
    };

    Device();

    ~Device();

    Device(Device&& other);
    Device& operator=(Device&& other);

    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

    //void setEventFilter(std::function<Event(Event)>);

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};

} // namespace midi
} // namespace paddock
