#include "events.hpp"
#include "utils.hpp"

#include "utils/overloaded.hpp"

#include <alsa/asoundlib.h>

#include <iostream>
#include <string>
#include <vector>

namespace paddock::midi::alsa
{
namespace
{
ClientId makeClientIdFromAlsaId(snd_seq_t* handle, int id)
{
    snd_seq_client_info_t* info;
    snd_seq_client_info_alloca(&info);
    snd_seq_get_any_client_info(handle, id, info);
    return makeClientId(info);
}
} // namespace

std::vector<std::byte> copyExtData(const snd_seq_event_t* event)
{
    auto dataPtr = static_cast<std::byte*>(event->data.ext.ptr);
    return std::vector<std::byte>{dataPtr, dataPtr + event->data.ext.len};
}

events::Event makeEvent(const snd_seq_event_t* event)
{
    switch (event->type)
    {
    case SND_SEQ_EVENT_NOTE:
        return events::Note{
            .channel = event->data.note.channel,
            .note = event->data.note.note,
            .velocity = event->data.note.velocity,
            .duration = static_cast<UValue14bit>(event->data.note.duration),
            .offVelocity = event->data.note.off_velocity};

    case SND_SEQ_EVENT_NOTEON:
        return events::NoteOn{.channel = event->data.note.channel,
                              .note = event->data.note.note,
                              .velocity = event->data.note.velocity};

    case SND_SEQ_EVENT_NOTEOFF:
        return events::NoteOff{.channel = event->data.note.channel,
                               .note = event->data.note.note,
                               .velocity = event->data.note.velocity};

    case SND_SEQ_EVENT_KEYPRESS:
        return events::KeyPressure{.channel = event->data.note.channel,
                                   .note = event->data.note.note,
                                   .velocity = event->data.note.velocity};

    case SND_SEQ_EVENT_CONTROLLER:
        return events::Controller{
            .channel = event->data.control.channel,
            .value = static_cast<Value7bit>(event->data.control.value),
            .parameter = static_cast<Value7bit>(event->data.control.param)};

    case SND_SEQ_EVENT_CONTROL14:
        return events::Controller14{
            .channel = event->data.control.channel,
            .value = static_cast<Value14bit>(event->data.control.value),

            .parameter = static_cast<Value7bit>(event->data.control.param)};
    case SND_SEQ_EVENT_PGMCHANGE:
        return events::ProgramChange{
            .channel = event->data.control.channel,
            .program = static_cast<Value7bit>(event->data.control.value)

        };

    case SND_SEQ_EVENT_CHANPRESS:
        return events::ChannelPressure{
            .channel = event->data.control.channel,
            .pressure = static_cast<Value7bit>(event->data.control.value)

        };

    case SND_SEQ_EVENT_PITCHBEND:
        return events::PitchBend{
            .channel = event->data.control.channel,
            .value = static_cast<Value14bit>(event->data.control.value)

        };

    case SND_SEQ_EVENT_NONREGPARAM:
        return events::NonRegParam{};

    case SND_SEQ_EVENT_REGPARAM:
        return events::RegParam{};

    case SND_SEQ_EVENT_SONGPOS:
        // Unimplemented
        return events::SongPosition{};

    case SND_SEQ_EVENT_SONGSEL:
        // Unimplemented
        return events::SelectSong{};

    case SND_SEQ_EVENT_QFRAME:
        // Unimplemented
        return events::QuarterFrame{};

    case SND_SEQ_EVENT_TIMESIGN:
        // Unimplemented
        return events::TimeSignature{};

    case SND_SEQ_EVENT_KEYSIGN:
        // Unimplemented
        return events::KeySignature{};

    case SND_SEQ_EVENT_START:
        // Unimplemented
        return events::Start{};

    case SND_SEQ_EVENT_CONTINUE:
        return events::Continue{};

    case SND_SEQ_EVENT_STOP:
        return events::Stop{};

    case SND_SEQ_EVENT_SETPOS_TICK:
        // Unimplemented
        return events::SetPositionTick{};

    case SND_SEQ_EVENT_SETPOS_TIME:
        // Unimplemented
        return events::SetPositionTime{};

    case SND_SEQ_EVENT_TEMPO:
        // Unimplemented
        return events::Tempo{};

    case SND_SEQ_EVENT_CLOCK:
        // Unimplementedq
        return events::Clock{};

    case SND_SEQ_EVENT_TICK:
        // Unimplemented
        return events::Tick{};

    case SND_SEQ_EVENT_QUEUE_SKEW:
        // Unimplemented
        return events::QueueSkew{};

    case SND_SEQ_EVENT_SYNC_POS:
        // Unimplemented
        return events::SyncPosition{};

    case SND_SEQ_EVENT_USR0:
    case SND_SEQ_EVENT_USR1:
    case SND_SEQ_EVENT_USR2:
    case SND_SEQ_EVENT_USR3:
    case SND_SEQ_EVENT_USR4:
    case SND_SEQ_EVENT_USR5:
    case SND_SEQ_EVENT_USR6:
    case SND_SEQ_EVENT_USR7:
    case SND_SEQ_EVENT_USR8:
    case SND_SEQ_EVENT_USR9:
        // Unimplemented
        return events::User{.number = event->type - SND_SEQ_EVENT_USR0};

    case SND_SEQ_EVENT_USR_VAR0:
    case SND_SEQ_EVENT_USR_VAR1:
    case SND_SEQ_EVENT_USR_VAR2:
    case SND_SEQ_EVENT_USR_VAR3:
    case SND_SEQ_EVENT_USR_VAR4:
        return events::UserVariable{.number =
                                        event->type - SND_SEQ_EVENT_USR_VAR0,
                                    .data = copyExtData(event)};

    case SND_SEQ_EVENT_SYSTEM:
        return events::System{.event = event->data.result.event,
                              .result = event->data.result.result};

    case SND_SEQ_EVENT_RESULT:
        return events::Result{.event = event->data.result.event,
                              .result = event->data.result.result};

    case SND_SEQ_EVENT_SYSEX:
        return events::SysEx{.data = copyExtData(event)};

    case SND_SEQ_EVENT_TUNE_REQUEST:
        return events::TuneRequest{};

    case SND_SEQ_EVENT_RESET:
        return events::Reset{};

    case SND_SEQ_EVENT_SENSING:
        return events::ActiveSensing{};

    case SND_SEQ_EVENT_ECHO:
        // Unimplemented
        return events::Echo{};

    case SND_SEQ_EVENT_BOUNCE:
        return events::Bounce{.data = copyExtData(event)};

    case SND_SEQ_EVENT_NONE:
        return events::None{};

    default:
        return events::Unknown{};
    }
}

std::optional<events::EngineEvent> makeEvent(
    const snd_seq_event_t* event, snd_seq_t* handle,
    const std::vector<ClientIds>& clients)
{
    using namespace midi::events;

    auto findClient = [&clients](int id) {
        for (auto client : clients)
        {
            if (std::get<int>(client) == id)
                return std::get<ClientId>(client);
        }
        return ClientId{};
    };

    switch (event->type)
    {
    case SND_SEQ_EVENT_CLIENT_START:
    {
        ClientStart client{
            makeClientIdFromAlsaId(handle, event->data.addr.client)};
        return EngineEvent{std::move(client)};
    }
    case SND_SEQ_EVENT_CLIENT_EXIT:
        return EngineEvent{ClientExit{findClient(event->data.addr.client)}};

    case SND_SEQ_EVENT_CLIENT_CHANGE:
        return EngineEvent{ClientChange{findClient(event->data.addr.client)}};

    case SND_SEQ_EVENT_PORT_START:
        return EngineEvent{PortStart{findClient(event->data.addr.client),
                                     event->data.addr.port}};

    case SND_SEQ_EVENT_PORT_EXIT:
        return EngineEvent{PortExit{findClient(event->data.addr.client),
                                    event->data.addr.port}};

    case SND_SEQ_EVENT_PORT_CHANGE:
        return EngineEvent{PortChange{findClient(event->data.addr.client),
                                      event->data.addr.port}};

    case SND_SEQ_EVENT_PORT_SUBSCRIBED:
        return EngineEvent{PortSubscribed{}}; // TODO

    case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
        return EngineEvent{PortUnsubscribed{}}; // TODO

    default:
        return std::nullopt;
    }
}

snd_seq_event_t makeEvent(const events::Event& event)
{
    return std::visit(
        overloaded{[](const auto& event) {
                       // This overload includes all events that will not be
                       // generated by this application.
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_NONE;
                       return output;
                   },

                   [](const events::Note& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_NOTE;
                       output.data.note.channel = event.channel;
                       output.data.note.note = event.note;
                       output.data.note.velocity = event.velocity;
                       output.data.note.off_velocity = event.offVelocity;
                       output.data.note.duration = event.duration;
                       return output;
                   },

                   [](const events::NoteOn& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_NOTEON;
                       output.data.note.channel = event.channel;
                       output.data.note.note = event.note;
                       output.data.note.velocity = event.velocity;
                       return output;
                   },

                   [](const events::NoteOff& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_NOTEOFF;
                       output.data.note.channel = event.channel;
                       output.data.note.note = event.note;
                       output.data.note.velocity = event.velocity;
                       return output;
                   },

                   [](const events::KeyPressure& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_KEYPRESS;
                       output.data.note.channel = event.channel;
                       output.data.note.note = event.note;
                       output.data.note.velocity = event.velocity;
                       return output;
                   },

                   [](const events::Controller& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_CONTROLLER;
                       output.data.control.channel = event.channel;
                       output.data.control.param = event.parameter;
                       output.data.control.value = event.value;
                       return output;
                   },

                   [](const events::Controller14& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_CONTROL14;
                       output.data.control.channel = event.channel;
                       output.data.control.param = event.parameter;
                       output.data.control.value = event.value;
                       return output;
                   },

                   [](const events::ProgramChange& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_PGMCHANGE;
                       output.data.control.channel = event.channel;
                       output.data.control.value = event.program;
                       return output;
                   },

                   [](const events::ChannelPressure& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_CHANPRESS;
                       output.data.control.channel = event.channel;
                       output.data.control.value = event.pressure;
                       return output;
                   },

                   [](const events::PitchBend& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_PITCHBEND;
                       output.data.control.channel = event.channel;
                       output.data.control.value = event.value;
                       return output;
                   },

                   [](const events::NonRegParam& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_NONREGPARAM;
                       output.data.control.channel = event.channel;
                       output.data.control.param = event.parameter;
                       output.data.control.value = event.value;
                       return output;
                   },

                   [](const events::RegParam& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_REGPARAM;
                       output.data.control.channel = event.channel;
                       output.data.control.param = event.parameter;
                       output.data.control.value = event.value;
                       return output;
                   },

                   [](const events::SongPosition& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SONGPOS;
                       output.data.queue.param.position = event.beats;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::SelectSong& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SONGSEL;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::QuarterFrame& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_QFRAME;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::TimeSignature& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_TIMESIGN;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::KeySignature& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_KEYSIGN;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::Start& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_START;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::Continue& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_CONTINUE;
                       return output;
                   },

                   [](const events::Stop& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_STOP;
                       return output;
                   },

                   [](const events::SetPositionTick& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SETPOS_TICK;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },
                   [](const events::SetPositionTime& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SETPOS_TIME;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },
                   [](const events::Tempo& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_TEMPO;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },
                   [](const events::Clock& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_CLOCK;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },
                   [](const events::Tick& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_TICK;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },
                   [](const events::QueueSkew& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_QUEUE_SKEW;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::SyncPosition& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SYNC_POS;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::User& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_USR0 + event.number;
                       throw std::runtime_error("Unimplemented");
                       return output;
                   },

                   [](const events::UserVariable& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_USR_VAR0 + event.number;
                       output.data.ext.len = event.data.size();
                       output.data.ext.ptr =
                           const_cast<std::byte*>(event.data.data());
                       return output;
                   },

                   [](const events::System& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SYSTEM;
                       output.data.result.event = event.event;
                       output.data.result.result = event.result;
                       return output;
                   },

                   [](const events::Result& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_RESULT;
                       output.data.result.event = event.event;
                       output.data.result.result = event.result;
                       return output;
                   },

                   [](const events::SysEx& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SYSEX;
                       output.data.ext.len = event.data.size();
                       output.data.ext.ptr =
                           const_cast<std::byte*>(event.data.data());
                       return output;
                   },

                   [](const events::TuneRequest& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_TUNE_REQUEST;
                       return output;
                   },

                   [](const events::Reset& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_RESET;
                       return output;
                   },

                   [](const events::ActiveSensing& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_SENSING;
                       return output;
                   },

                   [](const events::Echo& event) {
                       snd_seq_event_t output;
                       output.type = SND_SEQ_EVENT_ECHO;
                       return output;
                   }},

        event);
    return snd_seq_event_t{};
}

} // namespace paddock::midi::alsa
