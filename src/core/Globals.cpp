#include "Globals.hpp"

namespace paddock
{
namespace core
{
Globals& Globals::instance()
{
    static Globals globals;
    return globals;
}
} // namespace core
} // namespace paddock

