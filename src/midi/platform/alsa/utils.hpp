#pragma once

#include "midi/types.hpp"

#include <alsa/asoundlib.h>

namespace paddock
{
namespace midi
{
namespace alsa
{
using ClientIds = std::tuple<int, ClientId>;

ClientId makeClientId(snd_seq_client_info_t* info);

} // namespace alsa
} // namespace midi
} // namespace paddock
