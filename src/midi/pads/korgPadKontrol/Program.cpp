#include "Program.hpp"

#include "midi/Client.hpp"
#include "midi/eventPrinters.hpp"
#include "nativeEventPrinters.hpp"

#include <iostream>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
void Program::processEvent(const Event& event, Client& client, Device& device)
{
    std::cout << event << std::endl;
    (void)client;
    (void)device;
}

void Program::processEvent(const midi::events::Event& event, Client& client)
{
    std::cout << event << std::endl;
    client.postEvent(event);
}

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
