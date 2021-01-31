#include "Globals.hpp"

namespace paddock::core
{
Globals& Globals::instance()
{
    static Globals globals;
    return globals;
}
} // namespace paddock::core
