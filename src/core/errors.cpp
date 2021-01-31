#include "errors.hpp"

namespace paddock::core
{
namespace
{
struct PaddockErrorCategory : std::error_category
{
    const char* name() const noexcept override { return "paddock-error"; }

    std::string message(int value) const override
    {
        switch (static_cast<Error>(value))
        {
        case Error::unimplemented:
            return "Unimplemented";
        default:
            throw std::logic_error("Unknown error condition");
        }
    }
};
const PaddockErrorCategory paddockErrorCategory;

struct PaddockErrorTypeCategory : std::error_category
{
    const char* name() const noexcept override { return "paddock-error-type"; }

    std::string message(int value) const override
    {
        switch (static_cast<ErrorType>(value))
        {
        case ErrorType::core:
            return "core error";
        case ErrorType::program:
            return "device program error";
        case ErrorType::nsmSession:
            return "NSM session error";
        case ErrorType::midi:
            return "MIDI subsystem error";
        default:
            throw std::logic_error("Unknown error condition");
        }
    }
};
const PaddockErrorTypeCategory paddockErrorTypeCategory;

} // namespace

std::error_code make_error_code(Error error)
{
    return std::error_code{static_cast<int>(error), paddockErrorCategory};
}

std::error_condition make_error_condition(ErrorType error)
{
    return std::error_condition{static_cast<int>(error),
                                paddockErrorTypeCategory};
}

} // namespace paddock::core
