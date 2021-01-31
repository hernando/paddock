#pragma once

#include <variant>

#include "KorgPadKontrol.hpp"

namespace paddock::midi
{
using Pad = std::variant<KorgPadKontrol>;
}
