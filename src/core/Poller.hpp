#pragma once

#include <functional>
#include <future>
#include <memory>

namespace paddock
{
namespace core
{

/// An encapsulation of a platform specific handle for polling.
/// The identity of the handler is the value of the stored pointer.
/// The same pointer mustn't be added to Poller with a different callback.
using PollHandle = std::shared_ptr<void>;

// TODO make events platform independent
using PollCallback = std::function<void(const void* handle, int events)>;

class Poller
{
public:
    Poller();

    ~Poller();

    Poller(Poller&& other);
    Poller& operator=(Poller&& other);

    Poller(const Poller& other) = delete;
    Poller& operator=(const Poller& other) = delete;

    void add(PollHandle handle, PollCallback callback);

    /// Remove a handle from the poller
    /// @return a future that will wait for the removal to actually take
    /// place
    std::future<void> remove(const PollHandle& handle);

private:
    class _Impl;
    std::unique_ptr<_Impl> _impl;
};

} // namespace core
} // namespace paddock
