#pragma once

namespace paddock::core
{
class Globals
{
public:
    char** argv;
    int argc;

    static Globals& instance();
};
} // namespace paddock::core
