#pragma once

#include "../Poller.hpp"

#include "utils/Expected.hpp"

#include <chrono>
#include <span>
#include <system_error>

namespace paddock::core
{
struct PollDescriptor
{
    PollHandle handle;
    PollCallback callback;
};

/// Poll a list descriptors for requested events.
/// If an event has occurred in a descriptor, the event type is stored in
/// its associated obect and if a callback has been set, it gets called.
/// In POSIX the only recoverable error is EINTR, in all other cases an
/// exception is thrown, std::logic_error for EFAULT or EINVAL and
//  std::bad_alloc for ENOMEM.
/// @return the number of descriptors with events or an error.
Expected<unsigned int> poll(std::span<PollDescriptor>,
                            std::chrono::milliseconds timeout);

} // namespace paddock::core
