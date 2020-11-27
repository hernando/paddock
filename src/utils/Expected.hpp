#pragma once

#include <tl/expected.hpp>

#include <system_error>

namespace paddock
{
template <typename T>
using Expected = tl::expected<T, std::error_code>;
}
