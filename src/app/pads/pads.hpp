#pragma once

#include "KorgPadKontrol.hpp"

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

} // namespace paddock
