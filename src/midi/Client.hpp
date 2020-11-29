#pragma once

#include "PortInfo.hpp"

#include <memory>
#include <string>
#include <vector>

namespace paddock
{
namespace midi
{
struct ClientInfo
{
    std::string name;
    ClientType type;
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
