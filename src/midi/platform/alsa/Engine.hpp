#pragma once

#include "Sequencer.hpp"

#include "midi/Client.hpp"

#include <optional>

namespace paddock
{
namespace midi
{
namespace alsa
{
class Engine
{
public:
    Expected<Sequencer> openClient(const char* name, PortDirection direction);

    std::vector<ClientInfo> queryClientInfos() const;
    std::optional<ClientInfo> queryClientInfo(const ClientId& id) const;
};
} // namespace alsa
} // namespace midi
} // namespace paddock
