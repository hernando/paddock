#pragma once

#include "../poll.hpp"

#include <cassert>
#include <poll.h>
#include <string.h>

namespace paddock::core::posix
{
Expected<unsigned int> poll(std::span<PollDescriptor> descriptors,
                            std::chrono::milliseconds timeout)
{
    std::vector<struct pollfd> fds;
    fds.reserve(descriptors.size());
    for (auto descriptor : descriptors)
    {
        assert(descriptor.handle);
        fds.push_back(*static_cast<pollfd*>(descriptor.handle.get()));
    }

    switch (int result = ::poll(fds.data(), fds.size(), timeout.count()))
    {
    case -1:
        switch (errno)
        {
        case EINTR:
            return tl::unexpected(std::make_error_code(std::errc::interrupted));
        case EFAULT:
        case EINVAL:
            throw std::logic_error{strerror(errno)};
        case ENOMEM:
            throw std::bad_alloc{};
        }
    case 0:
        return 0;
    default:
        int count = 0;
        for (size_t i = 0; i != fds.size() && count != result; ++i)
        {
            auto& descriptor = descriptors[i];
            if (fds[i].revents != 0 && descriptor.callback)
            {
                descriptor.callback(descriptor.handle.get(), fds[i].revents);
            }
        }
        return result;
    }
}

} // namespace paddock::core::posix
