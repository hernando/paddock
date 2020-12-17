#pragma once

#include "PortInfo.hpp"
#include "events.hpp"

#include "utils/Expected.hpp"

#include <memory>
#include <string>
#include <vector>

namespace paddock
{
namespace midi
{
struct ClientInfo
{
    std::string name;
    ClientType type;
    std::vector<PortInfo> inputs;
    std::vector<PortInfo> outputs;

    ClientId id;
};

class AbstractClient;

// Class representing the Paddock client to the system sequencer
// This client is created with one input port to be connected to a hardware
// device and one output port to be connected to another application.
// This class is passive in the sense that it doesn't listen to the input
// port by any mechanism. Use midi::Engine to poll the input port.
class Client
{
public:
    friend class Engine;

    ~Client();
    Client(Client&& other);
    Client& operator=(Client&& other);

    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    const ClientInfo& info() const;

    std::error_code connectInput(const ClientInfo& other, unsigned int outPort);
    std::error_code connectOutput(const ClientInfo& other, unsigned int inPort);

    std::error_code postEvent(const events::Event& event);
    Expected<events::Event> readEvent();
    bool hasEvents() const;

    // Get a poll handle for in/out events.
    // The implementations must guarantee that the returned pointers
    // are always the same (to allow comparison).
    std::shared_ptr<void> pollHandle(PollEvents events) const;

private:
    template <typename T>
    class Model;
    std::unique_ptr<AbstractClient> _impl;

    template <typename T>
    Client(Model<T> impl);
};

} // namespace midi
} // namespace paddock
