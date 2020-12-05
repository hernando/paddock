#include "Poller.hpp"

#include "platform/poll.hpp"

#include <condition_variable>
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
        _condition.notify_one();
        _thread.join();
    }

    void add(PollDescriptor&& descriptor)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _descriptors.push_back(std::move(descriptor));
        _condition.notify_one();
    }

    std::future<void> remove(const PollHandle& handle)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        auto iter = std::remove_if(_descriptors.begin(), _descriptors.end(),
                           [&handle](const PollDescriptor& x) {
                               return x.handle == handle;
                           });

        // Don't return a waiting future if handle is not in the descriptors.
        if (iter == _descriptors.end())
        {
            std::promise<void> promise;
            auto future = promise.get_future();
            promise.set_value();
            return future;
        }

        _descriptors.erase(iter, _descriptors.end());

        _removalPromises.emplace_back();
        return _removalPromises.back().get_future();
    }

private:
    std::atomic_bool _isRunning{false};
    std::vector<PollDescriptor> _descriptors;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::vector<std::promise<void>> _removalPromises;
    std::thread _thread;
    bool _isThreadWaiting{false};

    void _runEventDispatcher()
    {
        while (_isRunning)
        {
            std::vector<PollDescriptor> descriptors;
            {
                std::unique_lock<std::mutex> lock(_mutex);

                for (auto& promise : _removalPromises)
                    promise.set_value();
                _removalPromises.clear();

                _isThreadWaiting = true;
                _condition.wait(lock, [this] {
                        return !_descriptors.empty() || !_isRunning; });
                _isThreadWaiting = false;
                // This copy wouldn't be efficient if a large number of
                // descriptors needs to be handled, but this is not our case.
                descriptors = _descriptors;
            };

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

void Poller::add(PollHandle handle, PollCallback callback)
{
    if (!handle)
        return;
    _impl->add(PollDescriptor{std::move(handle), std::move(callback)});
}

std::future<void> Poller::remove(const PollHandle& handle)
{
    return _impl->remove(handle);
}

} // namespace core
} // namespace paddock
