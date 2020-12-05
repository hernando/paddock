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
    virtual std::error_code connectInput(const ClientInfo& other,
                                         unsigned int inPort) = 0;
    virtual std::error_code connectOutput(const ClientInfo& other,
                                          unsigned int outPort) = 0;
    virtual bool hasEvents() = 0;
    virtual Expected<events::Event> readEvent() = 0;
    virtual std::error_code postEvent(const events::Event& event) = 0;
    virtual std::shared_ptr<void> pollHandle(PollEvents events) const = 0;
};

template <typename T>
class Client::Model : public AbstractClient
{
public:
    Model(T client)
        : _client(std::move(client))
    {
    }

    const ClientInfo& info() const final { return _client.info(); }

    std::error_code connectInput(const ClientInfo& other,
                                 unsigned int inPort) final
    {
        return _client.connectInput(other, inPort);
    }

    std::error_code connectOutput(const ClientInfo& other,
                                  unsigned int outPort) final
    {
        return _client.connectOutput(other, outPort);
    }

    bool hasEvents() final
    {
        return _client.hasEvents();
    }

    Expected<events::Event> readEvent()
    {
        return _client.readEvent();
    }

    std::error_code postEvent(const events::Event& event) final
    {
        return _client.postEvent(event);
    }

    std::shared_ptr<void> pollHandle(PollEvents events) const final
    {
        return _client.pollHandle(events);
    }

private:
    T _client;
}; // namespace midi

template <typename T>
inline Client::Client(Model<T> impl)
    : _impl(new Model<T>(std::move(impl)))
{
}

} // namespace midi
} // namespace paddock
