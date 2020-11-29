#include "Engine.hpp"

#include "ClientPrivate.hpp"

#include "core/errors.hpp"

#include "platform/alsa/Engine.hpp"
#include "platform/alsa/Sequencer.hpp"

namespace paddock
{
namespace midi
{
namespace
{
class MidiEngineErrorCategory : public std::error_category
{
    const char* name() const noexcept override { return "midi-engine-error"; }
    std::string message(int code) const override
    {
        using Error = Engine::Error;
        switch (static_cast<Error>(code))
        {
        case Error::NoEngineAvailable:
            return "No MIDI engine available";
        default:
            throw std::logic_error("Unknown error code");
        }
    }
    bool equivalent(int code, const std::error_condition& condition) const
        noexcept override
    {
        return (condition == core::ErrorType::MidiError);
    }
};

const MidiEngineErrorCategory midiEngineErrorCategory{};
} // namespace

std::error_code make_error_code(Engine::Error error)
{
    return std::error_code{static_cast<int>(error), midiEngineErrorCategory};
}

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

    virtual std::vector<ClientInfo> clientInfos()
    {
        return _engine.clientInfos();
    }

    virtual Expected<Client> openClient(const char* name)
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
    return make_error_code(Error::NoEngineAvailable);
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
