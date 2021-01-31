#pragma once

#include "Sequencer.hpp"

#include "midi/Client.hpp"
#include "midi/events.hpp"

#include <mutex>
#include <optional>

namespace paddock::midi::alsa
{
using ClientIds = std::tuple<int, ClientId>;

class Engine
{
public:
    static Expected<Engine> create();

    ~Engine();

    Engine(Engine&& other) noexcept;

    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine& operator=(Engine&& other) = delete;

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

    mutable std::mutex _clientMutex;
    std::vector<ClientIds> _clients;
    mutable std::optional<Expected<events::EngineEvent>> _nextEvent;

    Engine(Handle handle);
    std::optional<Expected<events::EngineEvent>> _extractEvent() const;
    void _processEvent(const events::EngineEvent& event);
    void _updateClientInfo(const ClientId& id);
};
} // namespace paddock::midi::alsa
