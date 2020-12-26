#pragma once

#include <system_error>

namespace paddock
{
namespace midi
{
enum class DeviceError
{
    noImplementationAvailable = 1,
    notReadable,
    notWritable,
    streamReadError
};

enum class EngineError
{
    clientIsNotADevice = 1,
    deviceNotReady,
    initializationFailed,
    noEngineAvailable,
    noDeviceFound,
    readEventFailed
};

enum class ProgramError
{
    invalidProgram = 1
};

std::error_code make_error_code(DeviceError error);
std::error_code make_error_code(EngineError error);
std::error_code make_error_code(ProgramError error);

} // namespace midi
} // namespace paddock

#include <system_error>

namespace std
{
template <>
struct is_error_code_enum<paddock::midi::DeviceError> : true_type
{
};
template <>
struct is_error_code_enum<paddock::midi::EngineError> : true_type
{
};
template <>
struct is_error_code_enum<paddock::midi::ProgramError> : true_type
{
};
} // namespace std
