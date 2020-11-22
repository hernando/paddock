#pragma once

namespace paddock
{
namespace core
{
class Globals
{
public:
    char** argv;
    int argc;

    static Globals& instance();
};
} // namespace core
} // namespace paddock
