#pragma once

#include <system_error>

namespace paddock
{
namespace core
{
enum class ErrorType
{
    NsmSessionError,
    MidiError
};

std::error_condition make_error_condition(ErrorType error);
} // namespace core
} // namespace paddock

namespace std
{
template <>
struct is_error_condition_enum<paddock::core::ErrorType> : true_type
{
};
} // namespace std
