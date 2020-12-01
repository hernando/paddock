#pragma once

#include <functional>
#include <memory>

namespace paddock
{
namespace core
{

/// An encapsulation of a platform specific handle for polling
/// The identity of the descriptor is the handle pointer. The same pointer
/// mustn't be added to Poller with a different callback.
struct PollDescriptor
{
    // TODO make events platform independent
    std::function<void(const void* handle, int events)> callback;
    std::shared_ptr<void> handle;

    bool operator==(const PollDescriptor& other) const
    {
        // should the callbacks be compared as well?
        return handle.get() == other.handle.get();
    }
};

class Poller
{
public:
    Poller();

    ~Poller();

    Poller(Poller&& other);
    Poller& operator=(Poller&& other);

    Poller(const Poller& other) = delete;
    Poller& operator=(const Poller& other) = delete;

    void add(PollDescriptor descriptor);
    void remove(const PollDescriptor& descriptor);

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};

} // namespace core
} // namespace paddock
