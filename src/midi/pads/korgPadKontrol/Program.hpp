#pragma once

#include "Scene.hpp"
#include "nativeEvents.hpp"

#include "midi/events.hpp"

#include <functional>
#include <optional>

namespace paddock::midi
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

    void setScene(Scene scene);
    /// Returns nullptr if no scene has been set yet
    const Scene* scene() const;

    void processEvent(const Event& event, Client& client, Device& device);

    void processEvent(const midi::events::Event& event, Client& client);

private:
    std::optional<Scene> _scene;
};

} // namespace korgPadKontrol
} // namespace paddock::midi
