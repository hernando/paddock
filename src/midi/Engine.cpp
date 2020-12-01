#include "Engine.hpp"

#include "ClientPrivate.hpp"
#include "errors.hpp"

#include "platform/alsa/Engine.hpp"
#include "platform/alsa/Sequencer.hpp"

#include <thread>

namespace paddock
{
namespace midi
{
class AbstractEngine
{
public:
    virtual ~AbstractEngine() {}
    virtual std::vector<ClientInfo> clientInfos() = 0;
    virtual Expected<Client> openClient(const char* name) = 0;
};

template <typename T>
class Engine::Model : public AbstractEngine
{
public:
    Model(T engine)
        : _engine(std::move(engine))
    {
    }

    std::vector<ClientInfo> clientInfos() final
    {
        return _engine.clientInfos();
    }

    Expected<Client> openClient(const char* name) final
    {
        auto client = _engine.openClient(name);
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
    return Engine{Model{alsa::Engine{}}};
#else
    return make_error_code(Error::noEngineAvailable);
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

Expected<Client> Engine::open(const char* name)
{
    return _impl->openClient(name);
}

std::vector<ClientInfo> Engine::clientInfos() const
{
    return _impl->clientInfos();
}

} // namespace midi
} // namespace paddock
