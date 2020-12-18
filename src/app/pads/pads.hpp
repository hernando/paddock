#pragma once

#include "KorgPadKontrol.hpp"

#include "midi/types.hpp"

#include "utils/Expected.hpp"

#include <variant>

namespace paddock
{
namespace midi
{
class Engine;
}

using Pad = std::variant<KorgPadKontrol*>;

Expected<Pad> makePad(QObject* parent, midi::Engine* engine,
                      std::string midiClientName);
Expected<Pad> makePad(QObject* parent, midi::Engine* engine,
                      std::string midiClientName,
                      const midi::ClientId& deviceId);
std::error_code tryReconnectPad(Pad& pad, midi::Engine* engine,
                                std::string midiClientName,
                                const midi::ClientId& deviceId);

} // namespace paddock
