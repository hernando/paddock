#include "Poller.hpp"

#include "platform/poll.hpp"

#include <mutex>
#include <thread>
#include <vector>

namespace paddock
{
namespace core
{
namespace
{
constexpr std::chrono::milliseconds _pollTimeOut{100};
}

class Poller::_Impl
{
public:
    _Impl()
        : _isRunning{true}
        , _thread{[this] { _runEventDispatcher(); }}
    {
    }

    virtual ~_Impl()
    {
        _isRunning = false;
        _thread.join();
    }

    void add(PollDescriptor&& descriptor)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _descriptors.push_back(std::move(descriptor));
    }

    void remove(const PollDescriptor& descriptor)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _descriptors.erase(
            std::remove_if(_descriptors.begin(), _descriptors.end(),
                           [&descriptor](const PollDescriptor& x) {
                               return x == descriptor;
                           }));
    }

private:
    std::atomic_bool _isRunning{false};
    std::mutex _mutex;
    std::thread _thread;
    std::vector<PollDescriptor> _descriptors;

    void _runEventDispatcher()
    {
        while (_isRunning)
        {
            // This copy wouldn't be efficient if a large number of
            // descriptors needs to be handled, but this is not our case.
            auto descriptors = [&]() {
                std::lock_guard<std::mutex> lock(_mutex);
                return _descriptors;
            }();

            // The only error that doesn't throw is when the operation
            // was interrupted, which we can safely ignore.
            poll(descriptors, _pollTimeOut);
        }
    }
};

Poller::Poller()
    : _impl{std::make_unique<_Impl>()}
{
}

Poller::~Poller() = default;

Poller::Poller(Poller&& other) = default;
Poller& Poller::operator=(Poller&& other) = default;

void Poller::add(PollDescriptor descriptor)
{
    _impl->add(std::move(descriptor));
}

void Poller::remove(const PollDescriptor& descriptor)
{
    _impl->remove(std::move(descriptor));
}

} // namespace core
} // namespace paddock
