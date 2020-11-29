#pragma once

#include "enums.hpp"

#include <memory>
#include <string>

namespace paddock
{
namespace midi
{

struct PortInfo
{
    std::string name;
    int number;

    PortDirection direction;
    PortType type;

    // Client ID of the ClientInfo to which this port belongs
    std::shared_ptr<void> clientId;

    // Hardware device identifier to use for opening a raw midi connection
    // to this port
    std::string hwDeviceId;
};

} // namespace midi
} // namespace paddock
