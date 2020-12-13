#include "Styling.hpp"

#include <QColor>

#define DECLARE_GROUP(container, name)          \
    auto name = new QQmlPropertyMap(container); \
    container->insert(QStringLiteral(#name), QVariant::fromValue(name));

#define DECLARE_STYLE_COLOR(group, name) \
    group->insert(QStringLiteral(#name), \
                  QVariant::fromValue(data.colors.group.name))
#define DECLARE_STYLE_SIZE(group, name)  \
    group->insert(QStringLiteral(#name), \
                  QVariant::fromValue(_scaling* data.sizes.group.name))
#define DECLARE_STYLE_SIZE2(name)         \
    _sizes->insert(QStringLiteral(#name), \
                   QVariant::fromValue(_scaling* data.sizes.name))

namespace paddock
{
namespace ui
{
struct StylingData
{
    struct Sizes
    {
        int onePoint{1};
        int twoPoints{2};

        struct Radii
        {
            int pad{4};
        } radii;

        struct Spacings
        {
            int min{4};
            int x2{8};
            int x3{12};
        } spacings;

        struct Widths
        {
            int pad{48};
        } widths;
    } sizes;

    struct Colors
    {
        struct Layers
        {
            QColor control{"#E7E7F0"};
            QColor background{"#FFFFE0"};
            QColor backgroundDisabled{"#FFFFE0"};
            QColor backgroundHovered{"#CFCFE0"};
            QColor border{"#303040"};
            QColor foreground{"#00000"};
            QColor foregroundHovered{"#00000"};
            QColor foregroundDisabled{"#7F7F7F"};
        } layers;

        struct State
        {
            QColor blinking{"#FF2020"};
        } state;
    } colors;
};

Styling::Styling()
    : _sizes(new QQmlPropertyMap(this))
    , _colors(new QQmlPropertyMap(this))
{
    StylingData data;

    // Colors
    DECLARE_GROUP(_colors, layers)
    DECLARE_STYLE_COLOR(layers, background);
    DECLARE_STYLE_COLOR(layers, backgroundHovered);
    DECLARE_STYLE_COLOR(layers, backgroundDisabled);
    DECLARE_STYLE_COLOR(layers, border);
    DECLARE_STYLE_COLOR(layers, control);
    DECLARE_STYLE_COLOR(layers, foreground);
    DECLARE_STYLE_COLOR(layers, foregroundHovered);
    DECLARE_STYLE_COLOR(layers, foregroundDisabled);

    DECLARE_GROUP(_colors, state)
    DECLARE_STYLE_COLOR(state, blinking);

    // Sizes
    DECLARE_STYLE_SIZE2(onePoint);
    DECLARE_STYLE_SIZE2(twoPoints);

    DECLARE_GROUP(_sizes, radii)
    DECLARE_STYLE_SIZE(radii, pad);

    DECLARE_GROUP(_sizes, spacings)
    DECLARE_STYLE_SIZE(spacings, min);
    DECLARE_STYLE_SIZE(spacings, x2);
    DECLARE_STYLE_SIZE(spacings, x3);

    DECLARE_GROUP(_sizes, widths)
    DECLARE_STYLE_SIZE(widths, pad);
}

QColor Styling::hovered(QColor color)
{
    const auto alter = [](int channel) {
        return channel > 224 ? channel - 12 : channel * 11 / 10;
    };

    return QColor(alter(color.red()), alter(color.green()), alter(color.blue()),
                  color.alpha());
}
} // namespace ui
} // namespace paddock
