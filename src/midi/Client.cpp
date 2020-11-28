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


} // namespace midi
} // namespace paddock
