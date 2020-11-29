#pragma once

#include "Sequencer.hpp"

#include "midi/Client.hpp"

namespace paddock
{
namespace midi
{
namespace alsa
{
class Engine
{
public:
    Expected<Sequencer> openClient(const char* name);

    std::vector<ClientInfo> clientInfos();
};
} // namespace alsa
} // namespace midi
} // namespace paddock
