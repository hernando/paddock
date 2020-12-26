#pragma once

#include <system_error>

namespace paddock
{
namespace core
{
enum class Error
{
    unimplemented
};

enum class ErrorType
{
    core,
    program,
    nsmSession,
    midi
};

std::error_code make_error_code(Error error);
std::error_condition make_error_condition(ErrorType error);
} // namespace core
} // namespace paddock

namespace std
{
template <>
struct is_error_code_enum<paddock::core::Error> : true_type
{
};
template <>
struct is_error_condition_enum<paddock::core::ErrorType> : true_type
{
};
} // namespace std
