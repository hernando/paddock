#pragma once

#include "Client.hpp"

#include "utils/Expected.hpp"

#include <memory>

namespace paddock
{
namespace midi
{

class AbstractEngine;
class ClientInfo;

class Engine
{
public:
    enum class Error
    {
        NoEngineAvailable = 1
    };
    template <typename T> class Model;

    static Expected<Engine> create();

    ~Engine();

    Engine(Engine&& other);
    Engine& operator=(Engine&& other);

    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;

    // Open a connection to the system sequencer.
    // @param name Client name
    Expected<Client> open(const char* name);

    // Get the infomation about all the clients in the system
    std::vector<ClientInfo> clientInfos() const;

private:
    std::unique_ptr<AbstractEngine> _impl;

    template <typename T>
    Engine(Model<T> impl);
};

} // namespace midi
} // namespace paddock
