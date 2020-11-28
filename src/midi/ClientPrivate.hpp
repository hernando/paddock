#pragma once

#include "Client.hpp"

namespace paddock
{
namespace midi
{

class AbstractClient
{
public:
    virtual ~AbstractClient() {}
    virtual const ClientInfo& info() const = 0;
};

template <typename T>
class Client::Model : public AbstractClient
{
public:
    Model(T client)
        : _client(std::move(client))
    {
    }

    virtual const ClientInfo& info() const
    {
        return _client.info();
    }

private:
    T _client;
};

template <typename T>
inline Client::Client(Model<T> impl)
    : _impl(new Model<T>(std::move(impl)))
{
}

}
}
