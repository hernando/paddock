#include "Engine.hpp"

#include "ClientPrivate.hpp"
#include "errors.hpp"

#include "pads/pads.hpp"

#include "platform/alsa/Engine.hpp"
#include "platform/alsa/Sequencer.hpp"

#include "core/Poller.hpp"

#include <thread>

namespace paddock
{
namespace midi
{
class AbstractEngine
{
public:
    AbstractEngine() = default;

    AbstractEngine(AbstractEngine&& other)
        : _poller(std::move(other._poller))
    {
    }

    virtual ~AbstractEngine() {}
    virtual std::vector<ClientInfo> queryClientInfos() const = 0;
    virtual std::optional<ClientInfo> queryClientInfo(
        const ClientId& id) const = 0;
    virtual Expected<Client> openClient(const std::string& name,
                                        PortDirection direction) = 0;

    void add(core::PollHandle&& handle, core::PollCallback&& callback)
    {
        _poller.add(std::move(handle), std::move(callback));
    }

    std::future<void> remove(const core::PollHandle&& handle)
    {
        return _poller.remove(handle);
    }

private:
    core::Poller _poller;
};

template <typename T>
class Engine::Model : public AbstractEngine
{
public:
    Model(T engine)
        : _engine(std::move(engine))
    {
    }

    std::vector<ClientInfo> queryClientInfos() const final
    {
        return _engine.queryClientInfos();
    }

    std::optional<ClientInfo> queryClientInfo(const ClientId& id) const final
    {
        return _engine.queryClientInfo(id);
    }

    Expected<Client> openClient(const std::string& name,
                                PortDirection direction) final
    {
        auto client = _engine.openClient(name.c_str(), direction);
        if (!client)
            return tl::unexpected(client.error());

        return Client{Client::Model(std::move(*client))};
    }

private:
    T _engine;
};

Expected<Engine> Engine::create()
{
#if PADDOCK_USE_ALSA
    return alsa::Engine::create().and_then(
        [](alsa::Engine&& engine) -> Expected<Engine> {
            return Engine{Model{std::move(engine)}};
        });
#else
    return tl::make_unexpected(EngineError::noEngineAvailable);
#endif
}

template <typename T>
Engine::Engine(Model<T> impl)
    : _impl(new Model<T>(std::move(impl)))
{
}

Engine::~Engine() = default;
Engine::Engine(Engine&& other) = default;
Engine& Engine::operator=(Engine&& other) = default;

std::vector<ClientInfo> Engine::queryClientInfos() const
{
    return _impl->queryClientInfos();
}

std::optional<ClientInfo> Engine::queryClientInfo(const ClientId& id) const
{
    return _impl->queryClientInfo(id);
}

Expected<Client> Engine::open(const std::string& name, PortDirection direction)
{
    return _impl->openClient(name, direction);
}

Expected<Pad> Engine::connect(const std::string& clientName)
{
    const auto infos = queryClientInfos();
    for (const auto& deviceInfo : infos)
    {
        if (KorgPadKontrol::matches(deviceInfo))
        {
            return KorgPadKontrol::open(this, deviceInfo, clientName)
                .and_then([](KorgPadKontrol&& pad) -> Expected<Pad> {
                    return std::move(pad);
                });
        }
    }
    return tl::make_unexpected(EngineError::noDeviceFound);
}

void Engine::add(core::PollHandle handle, core::PollCallback callback)
{
    _impl->add(std::move(handle), std::move(callback));
}

std::future<void> Engine::remove(const core::PollHandle& handle)
{
    return _impl->remove(std::move(handle));
}

} // namespace midi
} // namespace paddock
