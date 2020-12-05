#pragma once

#include "midi/events.hpp"

typedef struct snd_seq_event snd_seq_event_t;
typedef struct _snd_seq snd_seq_t;

namespace paddock
{
namespace midi
{
namespace alsa
{
events::Event makeEvent(const snd_seq_event_t* event, const snd_seq_t* handle);

// Make an ALSA seq event from an application event. Events that contain
// variale length data must be kept alive while snd_seq_event_t is only
// a pointer to the data will be stored.
snd_seq_event_t makeEvent(const events::Event& event);

} // namespace alsa
} // namespace midi
} // namespace paddock
