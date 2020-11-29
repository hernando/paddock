#pragma once

#include <memory>
#include <string>
#include <vector>

namespace paddock
{
namespace midi
{

struct PortInfo
{
    enum class Direction
    {
        read,
        write,
        duplex
    };

    enum class Type
    {
        hardware,
        software
    };

    std::string name;
    int number;

    Direction direction;
    Type type;

    // Client ID of the ClientInfo to which this port belongs
    std::shared_ptr<void> clientId;

    // Hardware device identifier to use for opening a raw midi connection
    // to this port
    std::string hwDeviceId;
};

struct ClientInfo
{
    enum class Type
    {
        user,
        system
    };

    std::string name;
    Type type;
    std::vector<PortInfo> inputs;
    std::vector<PortInfo> outputs;

    std::shared_ptr<void> id;
};

class AbstractClient;

class Client
{
public:
    friend class Engine;

    ~Client();
    Client(Client&& other);
    Client& operator=(Client&& other);

    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    const ClientInfo& info() const;

private:
    template <typename T> class Model;
    std::unique_ptr<AbstractClient> _impl;

    template <typename T>
    Client(Model<T> impl);
};

} // namespace midi
} // namespace paddock
