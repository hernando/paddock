#include "KorgPadKontrol.hpp"

#include "pads.hpp"

#include "midi/Engine.hpp"
#include "midi/errors.hpp"
#include "midi/pads/KorgPadKontrol.hpp"

#include "utils/overloaded.hpp"

namespace paddock
{
namespace
{
Expected<midi::ClientInfo> getDeviceInfo(midi::Engine* engine,
                                         const midi::ClientId& deviceId)
{
    auto info = engine->queryClientInfo(deviceId);
    if (!info)
        return tl::make_unexpected(midi::EngineError::noDeviceFound);
    if (info->type == midi::ClientType::user)
        return tl::make_unexpected(midi::EngineError::clientIsNotADevice);

    for (const auto& input : info->inputs)
    {
        if (input.hwDeviceId.empty())
            return tl::make_unexpected(midi::EngineError::deviceNotReady);
    }
    for (const auto& output : info->outputs)
    {
        if (output.hwDeviceId.empty())
            return tl::make_unexpected(midi::EngineError::deviceNotReady);
    }

    return std::move(*info);
}
} // namespace

Expected<Pad> makePad(QObject* parent, midi::Engine* engine,
                      std::string midiClientName)
{
    auto controller = engine->connect(std::move(midiClientName));
    if (!controller)
        return tl::make_unexpected(controller.error());

    return std::visit(
        overloaded{
            [parent](midi::KorgPadKontrol&& controller) -> Expected<Pad> {
                return new KorgPadKontrol{parent, std::move(controller)};
            }},
        std::move(*controller));
}

Expected<Pad> makePad(QObject* parent, midi::Engine* engine,
                      std::string midiClientName,
                      const midi::ClientId& deviceId)
{
    return getDeviceInfo(engine, deviceId)
        .and_then([&](const midi::ClientInfo& deviceInfo) {
            return engine->connect(std::move(midiClientName), deviceInfo);
        })
        .and_then([parent](midi::Pad&& controller) {
            return std::visit(
                overloaded{[parent](midi::KorgPadKontrol&& controller)
                               -> Expected<Pad> {
                    return new KorgPadKontrol{parent, std::move(controller)};
                }},
                std::move(controller));
        });
}

std::error_code tryReconnectPad(Pad& pad, midi::Engine* engine,
                                std::string midiClientName,
                                const midi::ClientId& deviceId)
{
    auto deviceInfo = getDeviceInfo(engine, deviceId);
    if (!deviceInfo)
        return deviceInfo.error();

    return std::visit(
        overloaded{[&](KorgPadKontrol* pad) {
            if (!midi::KorgPadKontrol::matches(*deviceInfo))
                return std::error_code{midi::EngineError::noDeviceFound};

            auto controller =
                midi::KorgPadKontrol::open(engine, *deviceInfo, midiClientName);
            if (!controller)
                return controller.error();
            return pad->setController(std::move(*controller));
        }},
        pad);
}

} // namespace paddock
