#pragma once

#include <QObject>

namespace paddock
{
namespace ControllerModel
{
    Q_NAMESPACE

    enum Model {
        None = 0,
        KorgPadKontrol
    };

    Q_ENUM_NS(Model)
};
}
