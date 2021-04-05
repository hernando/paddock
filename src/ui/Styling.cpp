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

namespace paddock::ui
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
            int x4{16};
        } spacings;

        struct Widths
        {
            int pad{120};
            int iconButton{24};
            int button{60};
        } widths;

        struct Heights
        {
            int button{36};
            int smallButton{24};
        } heights;

        struct Fonts
        {
            int title{16};
            int subtitle{14};
            int bigLabel{24};
        } fonts;
    } sizes;

    struct Colors
    {
        struct Layers
        {
            QColor control{"#E7E7EF"};
            QColor background{"#FFFFE0"};
            QColor backgroundDisabled{"#FFFFE0"};
            QColor border{"#303040"};
            QColor foreground{"#00000"};
            QColor foregroundDisabled{"#7F7F7F"};
        } layers;

        struct State
        {
            QColor blinking{"#FF2020"};
        } state;

        struct Buttons
        {
            QColor iconPressed{"#7F7F7F"};
            QColor iconChecked{"#1FAF1F"};
            QColor icon{"#000000"};
            QColor textPressed{"#7F7F7F"};
            QColor textChecked{"#1FAF1F"};
            QColor text{"#000000"};
        } buttons;
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
    DECLARE_STYLE_COLOR(layers, backgroundDisabled);
    DECLARE_STYLE_COLOR(layers, border);
    DECLARE_STYLE_COLOR(layers, control);
    DECLARE_STYLE_COLOR(layers, foreground);
    DECLARE_STYLE_COLOR(layers, foregroundDisabled);

    DECLARE_GROUP(_colors, state)
    DECLARE_STYLE_COLOR(state, blinking);

    DECLARE_GROUP(_colors, buttons)
    DECLARE_STYLE_COLOR(buttons, iconPressed);
    DECLARE_STYLE_COLOR(buttons, iconChecked);
    DECLARE_STYLE_COLOR(buttons, icon);
    DECLARE_STYLE_COLOR(buttons, textPressed);
    DECLARE_STYLE_COLOR(buttons, textChecked);
    DECLARE_STYLE_COLOR(buttons, text);

    // Sizes
    DECLARE_STYLE_SIZE2(onePoint);
    DECLARE_STYLE_SIZE2(twoPoints);

    DECLARE_GROUP(_sizes, radii)
    DECLARE_STYLE_SIZE(radii, pad);

    DECLARE_GROUP(_sizes, spacings)
    DECLARE_STYLE_SIZE(spacings, min);
    DECLARE_STYLE_SIZE(spacings, x2);
    DECLARE_STYLE_SIZE(spacings, x3);
    DECLARE_STYLE_SIZE(spacings, x4);

    DECLARE_GROUP(_sizes, widths)
    DECLARE_STYLE_SIZE(widths, pad);
    DECLARE_STYLE_SIZE(widths, iconButton);
    DECLARE_STYLE_SIZE(widths, button);

    DECLARE_GROUP(_sizes, heights)
    DECLARE_STYLE_SIZE(heights, button);
    DECLARE_STYLE_SIZE(heights, smallButton);

    DECLARE_GROUP(_sizes, fonts)
    DECLARE_STYLE_SIZE(fonts, title);
    DECLARE_STYLE_SIZE(fonts, subtitle);
    DECLARE_STYLE_SIZE(fonts, bigLabel);
}

QColor Styling::hovered(QColor color)
{
    const auto maxChannel =
        std::max(color.red(), std::max(color.green(), color.blue()));
    const auto alter = [](int channel, int maxValue) {
        return std::min(
            maxValue, channel > 231
                          ? channel - 12
                          : (channel > 182 ? channel * 11 / 10
                                           : std::max(channel * 14 / 10, 64)));
    };
    const auto maxValue = alter(maxChannel, 255);

    return QColor(alter(color.red(), maxValue), alter(color.green(), maxValue),
                  alter(color.blue(), maxValue), color.alpha());
}
} // namespace paddock::ui
