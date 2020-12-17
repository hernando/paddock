#pragma once

#include "Client.hpp"

#include "core/Poller.hpp"

#include "pads/pads.hpp"

#include "utils/Expected.hpp"

#include <memory>

namespace paddock
{
namespace midi
{
class AbstractEngine;
class ClientInfo;

// Callback to be called when a MIDI engine event is announced by
// the system. Will be called from another thread.
using EngineEventCallback = std::function<void(const events::EngineEvent&)>;

class Engine
{
public:
    template <typename T>
    class Model;

    static Expected<Engine> create();

    ~Engine();

    Engine(Engine&& other);
    Engine& operator=(Engine&& other);

    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;

    // Get the infomation about all the clients in the system
    std::vector<ClientInfo> queryClientInfos() const;

    std::optional<ClientInfo> queryClientInfo(const ClientId& id) const;

    /// Open a connection to the system sequencer.
    /// @param name Client name
    Expected<Client> open(const std::string& name, PortDirection direction);

    /// Connect to the first recognized hardware controller.
    /// @param clientName the name for the MIDI sequencer client
    Expected<Pad> connect(const std::string& clientName);

    void add(core::PollHandle handle, core::PollCallback callback);
    std::future<void> remove(const core::PollHandle& handle);

    void setEngineEventCallback(EngineEventCallback callback);

private:
    std::unique_ptr<AbstractEngine> _impl;

    template <typename T>
    Engine(T&& engine);
};

} // namespace midi
} // namespace paddock
