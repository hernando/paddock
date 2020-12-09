#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace paddock
{
std::vector<std::byte> to7bitEncoding(std::span<const std::byte> input);
std::vector<std::byte> from7bitEncoding(std::span<const std::byte> input);

} // namespace paddock
