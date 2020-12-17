#pragma once

#include "types.hpp"

#include "utils/mp.hpp"

#include <cstdint>
#include <variant>
#include <vector>

namespace paddock
{
namespace midi
{
namespace events
{
// Voice events

struct Note
{
    static constexpr auto description = "Note on and off with duration";
    Value7bit channel;
    Value7bit note;
    Value7bit velocity;
    UValue14bit duration; //  14 bit unsigned ?
    Value7bit offVelocity;
};

struct NoteOn
{
    static constexpr auto description = "Note on";
    Value7bit channel;
    Value7bit note;
    Value7bit velocity;
};

struct NoteOff
{
    static constexpr auto description = "Note off";
    Value7bit channel;
    Value7bit note;
    Value7bit velocity;
};

struct KeyPressure
{
    static constexpr auto description = "Key pressure change (aftertoch)";
    Value7bit channel;
    Value7bit note;
    Value7bit velocity;
};

// Control events

enum class ControllerParameter : unsigned char
{
    // Non exhaustive list
    ModulationWheel = 1,
    BreathController = 2,
    FootController = 4,
    PortamentoTime = 5,
    DataEntrySlider = 6, // Used by registered and non registered parameters
    MainVolume = 7,
    Balance = 8,
    Pan = 10,
    ExpressionController = 11,
    GeneralPurpose0 = 16,
    GeneralPurpose1 = 17,
    GeneralPurpose2 = 18,
    GeneralPurpose3 = 19,
    // From 32 to 63 LSB for controllers 0-31
    DamperPedal = 64,
    Portamento = 65,
    Sostenuto = 66,
    SoftPedal = 67,
    Hold = 69,
    GeneralPurpose4 = 80,
    GeneralPurpose5 = 81,
    GeneralPurpose6 = 82,
    GeneralPurpose7 = 83,
    TremoloDepth = 92,
    ChorusDepth = 93,
    CelesteDepth = 94,
    PhaserDepth = 95,
    DataIncrement = 96,
    DataDecrement = 97,
    NrpnLsb = 98, // Non-registered parameter number LSB
    NrpnMsb = 99, // Non-registered parameter number LSB
    RpnLsb = 100, // Registered parameter number LSB
    RpnMsb = 101, // Registered parameter number LSB
};

struct Controller
{
    static constexpr auto description = "Controller";
    Value7bit channel;
    Value7bit value;
    Value7bit parameter;
};

struct Controller14
{
    static constexpr auto description = "14 bit controller value";
    Value7bit channel;
    Value14bit value;
    Value7bit parameter;
};

struct ProgramChange
{
    static constexpr auto description = "Program change";
    Value7bit channel;
    Value7bit program;
};

struct ChannelPressure
{
    static constexpr auto description = "Channel pressure";
    Value7bit channel;
    Value7bit pressure;
};

struct PitchBend
{
    static constexpr auto description = "Pitch bend";
    Value7bit channel;
    Value14bit value;
};

struct NonRegParam
{
    static constexpr auto description = "14 bit NRPN";
    Value7bit channel;
    Value14bit parameter;
    Value14bit value;
};

// https://www.recordingblogs.com/wiki/midi-registered-parameter-number-rpn
// Registered parameter values in the IMA are:
// 0x0000 – Pitch bend range
// 0x0001 – Fine tuning
// 0x0002 – Coarse tuning
// 0x0003 – Tuning program change
// 0x0004 – Tuning bank select
// 0x0005 – Modulation depth range
struct RegParam
{
    static constexpr auto description = "14 bit RPN";
    Value7bit channel;
    Value14bit parameter;
    Value14bit value;
};

struct SongPosition
{
    static constexpr auto description = "SPP with LSB and MSB values";
    UValue14bit beats; // MIDI beats (1 beat = 6 MIDI clocks) since song start
};

struct SelectSong
{
    static constexpr auto description = "Song Select with song ID number";
    // SND_SEQ_EVENT_SONGSEL
    // event data type = snd_seq_ev_ctrl_t
};

struct QuarterFrame
{
    static constexpr auto description = "MIDI time code quarter frame";
    // SND_SEQ_EVENT_QFRAME
    // event data type = snd_seq_ev_ctrl_t
};

struct TimeSignature
{
    static constexpr auto description = "SMF Time Signature event";
    std::byte numerator;
    std::byte denominator;        // used as 2^(-d)
    std::byte ticksPerClick;      // MIDI clocks per metronome click
    std::byte notesPerQuaterNote; // 32nd notes per quater note
};

struct KeySignature
{
    static constexpr auto description = "SMF Key Signature event";
    std::byte sharps; // If negative it will be #flats
    bool major;
};

// Queue events

struct Start
{
    static constexpr auto description = "MIDI Real Time start message";
    // SND_SEQ_EVENT_START
    // event data type = snd_seq_ev_queue_control_t
};

struct Continue
{
    static constexpr auto description = "MIDI Real Time Continue message";
};

struct Stop
{
    static constexpr auto description = "MIDI Real Time Stop message";
};

struct SetPositionTick
{
    static constexpr auto description = "Set tick queue position";
    // SND_SEQ_EVENT_SETPOS_TICK
    // event data type = snd_seq_ev_queue_control_t
};

struct SetPositionTime
{
    static constexpr auto description = "Set real-time queue position";
    // SND_SEQ_EVENT_SETPOS_TIME
    // event data type = snd_seq_ev_queue_control_t
};

struct Tempo
{
    static constexpr auto description = "(SMF) Tempo event";
    unsigned int microsecsPerQuaterNote; // 24 bit
};

struct Clock
{
    static constexpr auto description = "MIDI Real Time Clock message";
    // SND_SEQ_EVENT_CLOCK
    // event data type = snd_seq_ev_queue_control_t
};

struct Tick
{
    static constexpr auto description = "MIDI Real Time Tick message";
    // SND_SEQ_EVENT_TICK
    // event data type = snd_seq_ev_queue_control_t
};

struct QueueSkew
{
    static constexpr auto description = "Queue timer skew";
    // SND_SEQ_EVENT_QUEUE_SKEW
    // event data type = snd_seq_ev_queue_control_t
};

struct SyncPosition
{
    static constexpr auto description = "Sync position changed";
    // SND_SEQ_EVENT_SYNC_POS
    // event data type = snd_seq_ev_queue_control_t
};

// User events

struct User
{
    static constexpr auto description = "user-defined event";
    int number; // From 0 to 9
    // SND_SEQ_EVENT_USR0 to SND_SEQ_EVENT_USR9
    // event data type = any (fixed size)
};

struct UserVariable
{
    static constexpr auto description = "user-defined event";
    int number; // From 0 to 4
    std::vector<std::byte> data;
};

// Result

struct System
{
    static constexpr auto description = "System status";
    int event;
    int result; // Not used?
};

struct Result
{
    static constexpr auto description = "Returned result status";
    int event;
    int result;
};

struct SysEx
{
    static constexpr auto description = "System exclusive data";
    std::vector<std::byte> data;
};

// Misc

struct TuneRequest
{
    static constexpr auto description = "Tune request";
};

struct Reset
{
    static constexpr auto description = "Reset to power-on state";
};

struct ActiveSensing
{
    static constexpr auto description = "Active sensing event";
};

struct Echo
{
    static constexpr auto description = "Echo-back event";
};

struct Bounce
{
    static constexpr auto description = "Error event";
    std::vector<std::byte> data;
};

struct None
{
    static constexpr auto description = "No operation";
};

struct Unknown
{
    static constexpr auto description = "Unknown event";
};

using VoiceTypes = mp::Types<Note, NoteOn, NoteOff, KeyPressure>;
using Voice = mp::apply<std::variant, VoiceTypes>;

using ControlTypes =
    mp::Types<Controller, Controller14, ProgramChange, ChannelPressure,
              PitchBend, NonRegParam, RegParam, SongPosition, SelectSong,
              QuarterFrame, TimeSignature, KeySignature>;
using Control = mp::apply<std::variant, ControlTypes>;

using QueueTypes =
    mp::Types<Start, Continue, Stop, SetPositionTick, SetPositionTime, Tempo,
              Clock, Tick, QueueSkew, SyncPosition>;
using Queue = mp::apply<std::variant, QueueTypes>;

using UserTypes = mp::Types<User, UserVariable>;
using UserEvent = mp::apply<std::variant, UserTypes>;

using SystemTypes = mp::Types<System, Result, SysEx>;
using SystemEvent = mp::apply<std::variant, SystemTypes>;

using MiscTypes =
    mp::Types<TuneRequest, Reset, ActiveSensing, Echo, Bounce, None, Unknown>;

using MidiTypes = mp::join<VoiceTypes, ControlTypes, QueueTypes,
                          UserTypes, SystemTypes, MiscTypes>;

using Event = mp::apply<std::variant, MidiTypes>;

// MIDI engine events

// TODO: move client/port handling to the Engine class

struct ClientStart
{
    static constexpr auto description = "New client has connected";
    ClientId client;
};

struct ClientExit
{
    static constexpr auto description = "Client has left the system";
    ClientId client;
};

struct ClientChange
{
    static constexpr auto description = "Client status/info has changed";
    ClientId client;
};

struct Address
{
    ClientId client;
    unsigned char port;
};

struct PortStart : public Address
{
    static constexpr auto description = "New port was created";
};

struct PortExit : public Address
{
    static constexpr auto description = "Port was deleted from system";
};

struct PortChange : public Address
{
    static constexpr auto description = "Port status/info has changed";
};

struct PortSubscribed
{
    static constexpr auto description = "Ports connected";
    Address source;
    Address destination;
};

struct PortUnsubscribed
{
    static constexpr auto description = "Ports disconnected";
    Address source;
    Address destination;
};

using EngineTypes =
    mp::Types<ClientStart, ClientExit, ClientChange, PortStart, PortExit,
              PortChange, PortSubscribed, PortUnsubscribed>;
using EngineEvent = mp::apply<std::variant, EngineTypes>;

} // namespace events
} // namespace midi
} // namespace paddock
