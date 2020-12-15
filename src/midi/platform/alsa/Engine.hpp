#pragma once

#include "Sequencer.hpp"

#include "midi/Client.hpp"
#include "midi/events.hpp"

#include <optional>

namespace paddock
{
namespace midi
{
namespace alsa
{
class Engine
{
public:
    static Expected<Engine> create();

    ~Engine();

    Engine(Engine&& other) noexcept;
    Engine& operator=(Engine&& other) noexcept;

    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;

    Expected<Sequencer> openClient(const char* name, PortDirection direction);

    std::vector<ClientInfo> queryClientInfos() const;
    std::optional<ClientInfo> queryClientInfo(const ClientId& id) const;

    std::shared_ptr<void> pollHandle() const;

    bool hasEvents() const;
    Expected<events::EngineEvent> readEvent();

private:
    using Handle = std::unique_ptr<snd_seq_t, int (*)(snd_seq_t*)>;
    using ClientInfoHandle =
        std::unique_ptr<snd_seq_client_info_t, int (*)(snd_seq_client_info_t*)>;

    Handle _handle;
    std::shared_ptr<void> _pollHandle;

    std::vector<std::tuple<int, ClientId>> _clients;

    Engine(Handle handle);
};
} // namespace alsa
} // namespace midi
} // namespace paddock
