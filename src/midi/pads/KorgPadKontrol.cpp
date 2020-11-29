#include "KorgPadKontrol.hpp"

#include "midi/Client.hpp"

#include <iostream>

namespace paddock
{
namespace midi
{
bool KorgPadKontrol::matches(const ClientInfo& clientInfo)
{
    return (clientInfo.name == "padKONTROL" &&
            clientInfo.type == ClientType::system &&
            clientInfo.outputs.size() == 3 &&
            clientInfo.inputs.size() == 2);
}
} // namespace midi
} // namespace paddock
