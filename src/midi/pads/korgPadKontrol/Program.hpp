#pragma once

#include "nativeEvents.hpp"
#include "midi/events.hpp"

#include <functional>

namespace paddock
{
namespace midi
{
class Client;
class Device;

namespace korgPadKontrol
{

class Program
{
public:
    using EventNotifier = std::function<void(const Event&)>;

    EventNotifier eventNotifier;

    void processEvent(const Event& event, Client& client, Device& device);

    void processEvent(const midi::events::Event& event, Client& client);
};

}
}
}
