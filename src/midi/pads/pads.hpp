#pragma once

#include <variant>

#include "KorgPadKontrol.hpp"

namespace paddock
{
namespace midi
{

using Pad = std::variant<KorgPadKontrol>;

}
}
