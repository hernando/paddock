#pragma once

#include "midi/Client.hpp"

#include "utils/Expected.hpp"

#include <memory>
#include <system_error>
#include <thread>

struct _snd_seq;
typedef struct _snd_seq snd_seq_t;

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
        OpenSequencerFailed = 1,
        SetClientNameFailed,
        PortCreationFailed
    };

    static Expected<Sequencer> open(const char* clientName);

    ~Sequencer();

    Sequencer(Sequencer&& other) noexcept;
    Sequencer& operator=(Sequencer&& other) noexcept;

    Sequencer(const Sequencer& other) = delete;
    Sequencer& operator=(const Sequencer& other) = delete;

    const ClientInfo& info() const;

private:
    using SeqHandle = std::unique_ptr<snd_seq_t, int (*)(snd_seq_t*)>;

    class _Impl;
    std::unique_ptr<_Impl> _impl;

    Sequencer(std::unique_ptr<_Impl> impl);
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
