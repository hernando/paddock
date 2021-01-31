#include "poll.hpp"

#include "posix/poll.hpp"

namespace paddock::core
{
Expected<unsigned int> poll(std::span<PollDescriptor> descriptors,
                            std::chrono::milliseconds timeout)
{
#ifdef Linux
    return posix::poll(descriptors, timeout);
#else
    return std::error_code(Error::unimplemented);
#endif
}

} // namespace paddock::core
