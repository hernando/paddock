#include "errors.hpp"

namespace paddock
{
namespace core
{
namespace
{
struct PaddockErrorCategory : std::error_category
{
    const char* name() const noexcept override { return "paddock-error"; }

    std::string message(int value) const override
    {
        switch (static_cast<ErrorType>(value))
        {
        case ErrorType::NsmSessionError:
            return "NSM session error";
        default:
            throw std::logic_error("Unknown error condition");
        }
    }

};

const PaddockErrorCategory paddockErrorCategory;
} // namespace

std::error_condition make_error_condition(ErrorType error)
{
    return std::error_condition{static_cast<int>(error),
            paddockErrorCategory};

}

} // namespace core
} // namespace paddock
