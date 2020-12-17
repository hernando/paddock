#include "Sequencer.hpp"

#include "events.hpp"

#include "core/Poller.hpp"
#include "core/errors.hpp"

#include <alsa/asoundlib.h>

namespace paddock
{
namespace midi
{
namespace alsa
{
namespace
{
class AlsaSeqErrorCategory : public std::error_category
{
    const char* name() const noexcept override { return "alsa-seq-error"; }
    std::string message(int code) const override
    {
        using Error = Sequencer::Error;
        switch (static_cast<Error>(code))
        {
        case Error::openSequencerFailed:
            return "Could not open ALSA sequencer connection";
        case Error::setClientNameFailed:
            return "Error setting ALSA sequencer client name";
        case Error::portCreationFailed:
            return "Could not create ALSA sequencer client port";
        case Error::portSubscriptionFailed:
            return "Could not subscribe to ALSA sequencer client port";
        case Error::readEventFailed:
            return "Error reading MIDI event";
        case Error::writeEventFailed:
            return "Error writing MIDI event";
        default:
            throw std::logic_error("Unknown error code");
        }
    }
    bool equivalent(int code, const std::error_condition& condition) const
        noexcept override
    {
        return (condition == core::ErrorType::midi);
    }
};

const AlsaSeqErrorCategory alsaSeqErrorCategory{};

constexpr bool isRead(PortDirection direction)
{
    return direction == PortDirection::read ||
           direction == PortDirection::duplex;
}

constexpr bool isWrite(PortDirection direction)
{
    return direction == PortDirection::write ||
           direction == PortDirection::duplex;
}

std::shared_ptr<void> getPollDescriptor(snd_seq_t* client, short events)
{
    // This function return always 1 if events has either POLLIN or POLLOUT.
    // Let's assume that's the case, but throw if it's not.
    if (snd_seq_poll_descriptors_count(client, events) != 1)
        throw std::logic_error("Poll descriptors is not 1");

    auto fd = std::make_shared<pollfd>();
    // No need to check the return value, it must be one.
    snd_seq_poll_descriptors(client, fd.get(), 1, events);
    return std::static_pointer_cast<void>(fd);
}
} // namespace

std::error_code make_error_code(Sequencer::Error error)
{
    return std::error_code{static_cast<int>(error), alsaSeqErrorCategory};
}

tl::expected<Sequencer, std::error_code> Sequencer::open(
    const char* clientName, PortDirection direction)
{
    using Error = Sequencer::Error;

    snd_seq_t* handle;
    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == -1)
        return tl::make_unexpected(Error::openSequencerFailed);

    Handle seqHandle(handle, snd_seq_close);

    if (snd_seq_set_client_name(handle, clientName) == -1)
        return tl::make_unexpected(Error::setClientNameFailed);

    const auto createPort = [handle](const char* name, int capabilities,
                                     int type) -> std::optional<PortInfo> {
        int port = snd_seq_create_simple_port(handle, name, capabilities, type);
        if (port == -1)
            return std::nullopt;
        return PortInfo{.name = name, .number = port};
    };

    std::vector<PortInfo> input;
    if (isWrite(direction))
    {
        auto inPort =
            createPort("paddock:in",
                       SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
                       SND_SEQ_PORT_TYPE_APPLICATION);
        if (!inPort)
            return tl::unexpected(make_error_code(Error::portCreationFailed));
        input.push_back(PortInfo{std::move(*inPort)});
    }

    std::vector<PortInfo> output;
    if (isRead(direction))
    {
        auto outPort =
            createPort("paddock:out",
                       SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
                       SND_SEQ_PORT_TYPE_PORT);
        if (!outPort)
            return tl::unexpected(make_error_code(Error::portCreationFailed));
        output.push_back(PortInfo{std::move(*outPort)});
    }

    ClientInfo info{.name = clientName,
                    .inputs = std::move(input),
                    .outputs = std::move(output)};

    return Sequencer(std::move(seqHandle), std::move(info));
}

Sequencer::Sequencer(Handle handle, ClientInfo info)
    : _handle{std::move(handle)}
    , _clientInfo{std::move(info)}
    , _inPollHandle{_clientInfo.inputs.size()
                        ? getPollDescriptor(_handle.get(), POLLIN)
                        : core::PollHandle{}}
    , _outPollHandle{_clientInfo.outputs.size()
                         ? getPollDescriptor(_handle.get(), POLLOUT)
                         : core::PollHandle{}}
{
}

Sequencer::~Sequencer()
{
    for (const auto& port : _clientInfo.inputs)
        snd_seq_delete_port(_handle.get(), port.number);
    for (const auto& port : _clientInfo.outputs)
        snd_seq_delete_port(_handle.get(), port.number);
}

Sequencer::Sequencer(Sequencer&& other) noexcept = default;
Sequencer& Sequencer::operator=(Sequencer&& other) noexcept = default;

const ClientInfo& Sequencer::info() const
{
    return _clientInfo;
}

std::error_code Sequencer::connectInput(const ClientInfo& source,
                                        unsigned int outPort)
{
    snd_seq_client_info_t* info =
        static_cast<snd_seq_client_info_t*>(source.id.get());

    snd_seq_addr_t sender;
    sender.client = snd_seq_client_info_get_client(info);
    sender.port = outPort;

    snd_seq_addr_t dest;
    dest.client = snd_seq_client_id(_handle.get());
    dest.port = _clientInfo.inputs[0].number;

    snd_seq_port_subscribe_t* subs;
    snd_seq_port_subscribe_alloca(&subs);
    snd_seq_port_subscribe_set_sender(subs, &sender);
    snd_seq_port_subscribe_set_dest(subs, &dest);
    snd_seq_port_subscribe_set_queue(subs, 1);
    snd_seq_port_subscribe_set_time_update(subs, 1);
    snd_seq_port_subscribe_set_time_real(subs, 1);
    if (snd_seq_subscribe_port(_handle.get(), subs) == -1)
        return Error::portSubscriptionFailed;

//    sender.client = SND_SEQ_CLIENT_SYSTEM;
//    sender.port = SND_SEQ_PORT_SYSTEM_ANNOUNCE;
//    dest.client = snd_seq_client_id(_handle.get());
//    dest.port = _clientInfo.inputs[0].number;
//    snd_seq_port_subscribe_set_sender(subs, &sender);
//    snd_seq_port_subscribe_set_dest(subs, &dest);
//    snd_seq_port_subscribe_set_queue(subs, 1);
//    snd_seq_port_subscribe_set_time_update(subs, 1);
//    snd_seq_port_subscribe_set_time_real(subs, 1);
//    if (snd_seq_subscribe_port(_handle.get(), subs) == -1)
//        return Error::portSubscriptionFailed;

    // TODO check errors;
    return std::error_code{};
}

std::error_code Sequencer::connectOutput(const ClientInfo& destination,
                                         unsigned int)
{
    return std::error_code{};
}

std::shared_ptr<void> Sequencer::pollHandle(PollEvents events) const
{
    switch (events)
    {
    case PollEvents::in:
        return _inPollHandle;
    case PollEvents::out:
        return _outPollHandle;
    default:
        throw std::logic_error("invalid value");
    }
}

bool Sequencer::hasEvents() const
{
    return snd_seq_event_input_pending(_handle.get(), 1) > 0;
}

Expected<events::Event> Sequencer::readEvent()
{
    snd_seq_event_t* event;
    // This result should tell us if there are events remaining in the buffer
    // but in reality it always returns 1 in case of success
    auto result = snd_seq_event_input(_handle.get(), &event);
    if (result < 0)
        return tl::make_unexpected(Error::readEventFailed);
    return makeEvent(event);
}

std::error_code Sequencer::postEvent(const events::Event& event)
{
    if (std::holds_alternative<events::Unknown>(event))
        return std::error_code{}; // Return an error?
    auto seqEvent = makeEvent(event);
    return _postEvent(&seqEvent);
}

std::error_code Sequencer::_postEvent(snd_seq_event_t* event)
{
    snd_seq_ev_set_source(event, _clientInfo.outputs[0].number);
    snd_seq_ev_set_subs(event);
    snd_seq_ev_set_direct(event);
    snd_seq_event_output(_handle.get(), event);
    snd_seq_drain_output(_handle.get());

    return std::error_code{};
}

} // namespace alsa
} // namespace midi
} // namespace paddock
