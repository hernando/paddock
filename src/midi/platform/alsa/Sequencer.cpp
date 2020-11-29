#include "Sequencer.hpp"

#include "core/errors.hpp"

#include <alsa/asoundlib.h>

#include <iostream>

namespace paddock
{
namespace midi
{
namespace alsa
{
namespace
{
constexpr int _pollTimeOut = 100; // Milliseconds

class AlsaSeqErrorCategory : public std::error_category
{
    const char* name() const noexcept override { return "alsa-seq-error"; }
    std::string message(int code) const override
    {
        using Error = Sequencer::Error;
        switch (static_cast<Error>(code))
        {
        case Error::OpenSequencerFailed:
            return "Could not open ALSA sequencer connection";
        case Error::SetClientNameFailed:
            return "Error setting ALSA sequencer client name";
        case Error::PortCreationFailed:
            return "Could not create ALSA sequencer client port";
        default:
            throw std::logic_error("Unknown error code");
        }
    }
    bool equivalent(int code, const std::error_condition& condition) const
        noexcept override
    {
        return (condition == core::ErrorType::MidiError);
    }
};

const AlsaSeqErrorCategory alsaSeqErrorCategory{};

std::error_code make_error_code(Sequencer::Error error)
{
    return std::error_code{static_cast<int>(error), alsaSeqErrorCategory};
}

using SeqHandle = std::unique_ptr<snd_seq_t, int (*)(snd_seq_t*)>;
} // namespace

tl::expected<Sequencer, std::error_code> Sequencer::open(const char* clientName)
{
    using Error = Sequencer::Error;

    snd_seq_t* handle;
    if (snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0) == -1)
        return tl::unexpected(make_error_code(Error::OpenSequencerFailed));

    SeqHandle seqHandle(handle, snd_seq_close);

    if (snd_seq_set_client_name(handle, clientName) == -1)
        return tl::unexpected(make_error_code(Error::SetClientNameFailed));

    const auto createPort = [handle](const char* name, int capabilities,
                                     int type) -> std::optional<PortInfo> {
        int port = snd_seq_create_simple_port(handle, name, capabilities, type);
        if (port == -1)
            return std::nullopt;
        return PortInfo{.name = name, .number = port};
    };

    auto inPort =
        createPort("paddock:in",
                   SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
                   SND_SEQ_PORT_TYPE_APPLICATION);

    auto outPort =
        createPort("paddock:out",
                   SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
                   SND_SEQ_PORT_TYPE_PORT);

    if (!inPort || !outPort)
        return tl::unexpected(make_error_code(Error::PortCreationFailed));

    ClientInfo info{.name = clientName,
                    .inputs = {PortInfo{std::move(*inPort)}},
                    .outputs = {PortInfo{std::move(*outPort)}}};

    return Sequencer(
        std::make_unique<_Impl>(std::move(seqHandle), std::move(info)));
}

class Sequencer::_Impl
{
public:
    SeqHandle handle;
    ClientInfo clientInfo;

    _Impl(SeqHandle handle_, ClientInfo info)
        : handle{std::move(handle_)}
        , clientInfo(std::move(info))
        , _isRunning{true}
        , _thread{[this] { _runEventFilter(); }}
    {
        snd_seq_addr_t sender, dest;
        snd_seq_port_subscribe_t* subs;
        sender.client = 24;
        sender.port = 1;
        dest.client = snd_seq_client_id(handle.get());
        dest.port = clientInfo.inputs[0].number;
        snd_seq_port_subscribe_alloca(&subs);
        snd_seq_port_subscribe_set_sender(subs, &sender);
        snd_seq_port_subscribe_set_dest(subs, &dest);
        snd_seq_port_subscribe_set_queue(subs, 1);
        snd_seq_port_subscribe_set_time_update(subs, 1);
        snd_seq_port_subscribe_set_time_real(subs, 1);
        snd_seq_subscribe_port(handle.get(), subs);

        snd_seq_connect_to(handle.get(), clientInfo.outputs[1].number, 24, 1);
    }

    ~_Impl()
    {
        _isRunning = false;
        _thread.join();
        for (const auto& port : clientInfo.inputs)
            snd_seq_delete_port(handle.get(), port.number);
        for (const auto& port : clientInfo.outputs)
            snd_seq_delete_port(handle.get(), port.number);
        handle.reset();
    }

private:
    std::atomic_bool _isRunning{false};
    std::thread _thread;

    void _runEventFilter()
    {
        std::vector<pollfd> fds;
        fds.resize(snd_seq_poll_descriptors_count(handle.get(), POLLIN));

        snd_seq_poll_descriptors(handle.get(), fds.data(), fds.size(), POLLIN);
        while (_isRunning)
        {
            if (poll(fds.data(), fds.size(), _pollTimeOut) > 0)
            {
                do
                {
                    snd_seq_event_t* event;
                    snd_seq_event_input(handle.get(), &event);
                    _processEvent(event);
                } while (snd_seq_event_input_pending(handle.get(), 0) > 0);
            }
        }
    }

    void _processEvent(snd_seq_event_t* event)
    {
        std::cout << event << std::endl;

        if ((event->type == SND_SEQ_EVENT_NOTEON) ||
            (event->type == SND_SEQ_EVENT_NOTEOFF))
        {
            const char* type =
                (event->type == SND_SEQ_EVENT_NOTEON) ? "on " : "off";
            printf("[%d] Note %s: %2x vel(%2x)\n", event->time.tick, type,
                   event->data.note.note, event->data.note.velocity);
        }
        else if (event->type == SND_SEQ_EVENT_CONTROLLER)
        {
            printf("[%d] Control:  %2x val(%2x)\n", event->time.tick,
                   event->data.control.param, event->data.control.value);
        }
        else
        {
            printf("[%d] Unknown:  Unhandled Event Received %d\n",
                   event->time.tick, (int)event->type);
        }
        _forwardEvent(event);
    }

    void _forwardEvent(snd_seq_event_t* event)
    {
        snd_seq_ev_set_source(event, clientInfo.outputs[0].number);
        snd_seq_ev_set_subs(event);
        snd_seq_ev_set_direct(event);
        snd_seq_event_output(handle.get(), event);
        snd_seq_drain_output(handle.get());
    }
};

Sequencer::Sequencer(std::unique_ptr<_Impl> impl)
    : _impl(std::move(impl))
{
}

Sequencer::~Sequencer() = default;
Sequencer::Sequencer(Sequencer&& other) noexcept = default;
Sequencer& Sequencer::operator=(Sequencer&& other) noexcept = default;

const ClientInfo& Sequencer::info() const
{
    return _impl->clientInfo;
}

} // namespace alsa
} // namespace midi
} // namespace paddock
