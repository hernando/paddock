#pragma once

#include "midi/Client.hpp"
#include "midi/events.hpp"

#include "utils/Expected.hpp"

#include <alsa/asoundlib.h>

#include <memory>
#include <system_error>
#include <thread>

namespace paddock
{
namespace midi
{
namespace alsa
{
class Sequencer
{
public:
    enum class Error
    {
        openSequencerFailed = 1,
        setClientNameFailed,
        portCreationFailed,
        portSubscriptionFailed,
        readEventFailed, // This can happen if the input buffer overran
        writeEventFailed
    };

    static Expected<Sequencer> open(const char* clientName,
                                    PortDirection direction);

    ~Sequencer();

    Sequencer(Sequencer&& other) noexcept;
    Sequencer& operator=(Sequencer&& other) noexcept;

    Sequencer(const Sequencer& other) = delete;
    Sequencer& operator=(const Sequencer& other) = delete;

    const ClientInfo& info() const;

    std::error_code connectInput(const ClientInfo& other, unsigned int outPort);
    std::error_code connectOutput(const ClientInfo& other, unsigned int inPort);

    std::shared_ptr<void> pollHandle(PollEvents events) const;

    bool hasEvents() const;
    Expected<events::Event> readEvent();
    std::error_code postEvent(const events::Event& event);

private:
    using Handle = std::unique_ptr<snd_seq_t, int (*)(snd_seq_t*)>;

    Handle _handle;
    ClientInfo _clientInfo;

    std::shared_ptr<void> _inPollHandle;
    std::shared_ptr<void> _outPollHandle;

    bool _hasEvents{false};

    Sequencer(Handle handle, ClientInfo info);

    std::error_code _postEvent(snd_seq_event_t* event);
};

} // namespace alsa
} // namespace midi
} // namespace paddock

#include <system_error>

namespace std
{
template <>
struct is_error_code_enum<paddock::midi::alsa::Sequencer::Error> : true_type
{
};
} // namespace std
