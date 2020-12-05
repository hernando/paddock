#include "KorgPadKontrol.hpp"

#include "pads.hpp"

#include "midi/Engine.hpp"
#include "midi/pads/KorgPadKontrol.hpp"

#include "utils/overloaded.hpp"

#include <iostream>

namespace paddock
{
Expected<Pad> makePad(QObject* parent, midi::Engine* engine,
                      std::string midiClientName)
{
    auto controller = engine->connect(std::move(midiClientName));
    if (!controller)
        return tl::make_unexpected(controller.error());

    return std::visit(overloaded{
        [parent](midi::KorgPadKontrol&& controller) -> Expected<Pad> {
            return new KorgPadKontrol{parent, std::move(controller)};
        }},
        std::move(*controller));
}

} // namespace paddock
