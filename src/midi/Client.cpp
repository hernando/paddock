#include "Client.hpp"
#include "ClientPrivate.hpp"

namespace paddock
{
namespace midi
{
Client::~Client() = default;
Client::Client(Client&& other) = default;
Client& Client::operator=(Client&& other) = default;

const ClientInfo& Client::info() const
{
    return _impl->info();
}

std::error_code Client::connectInput(const ClientInfo& other,
                                     unsigned int outPort)
{
    return _impl->connectInput(other, outPort);
}

std::error_code Client::connectOutput(const ClientInfo& other,
                                      unsigned int inPort)
{
    return _impl->connectOutput(other, inPort);
}

bool Client::hasEvents() const
{
    return _impl->hasEvents();
}

Expected<events::Event> Client::readEvent()
{
    return _impl->readEvent();
}

std::error_code Client::postEvent(const events::Event& event)
{
    return _impl->postEvent(event);
}

std::shared_ptr<void> Client::pollHandle(PollEvents events) const
{
    return _impl->pollHandle(events);
}

} // namespace midi
} // namespace paddock
