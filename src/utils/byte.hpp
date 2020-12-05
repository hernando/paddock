#pragma once

#include <cstddef>

namespace paddock
{
constexpr std::byte operator"" _b(unsigned long long c)
{
    return std::byte{static_cast<unsigned char>(c)};
}

} // namespace paddock
