#pragma once

#include <memory>

namespace paddock
{
namespace midi
{
using Value7bit = unsigned char;
using Value14bit = short;
using UValue14bit = unsigned short;

using ClientId = std::shared_ptr<void>;

} // namespace midi
} // namespace paddock
