#pragma once

namespace paddock
{
namespace midi
{

enum class ClientType
{
    user,
    system
};

enum class PortDirection
{
    read,
    write,
    duplex
};

enum class PortType
{
    hardware,
    software
};

}
}
