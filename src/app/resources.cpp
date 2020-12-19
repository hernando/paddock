#include "resources.hpp"

#include "Session.hpp"
#include "pads/KorgPadKontrol.hpp"
#include "pads/KorgPadKontrolProgram.hpp"

#include <QtQml/qqml.h>

namespace paddock
{
void initResources()
{
    qmlRegisterUncreatableType<KorgPadKontrolProgram>(
        "Paddock", 1, 0, "KorgPadKontrolProgram", "Not creatable in QML");
    qmlRegisterUncreatableType<Program>("Paddock", 1, 0, "Program",
                                        "Not creatable in QML");
    qmlRegisterUncreatableType<Session>("Paddock", 1, 0, "Session",
                                        "Not creatable in QML");
    qmlRegisterUncreatableType<KorgPadKontrol>(
        "Paddock", 1, 0, "KorgPadKontrol", "Not creatable in QML");
    qmlRegisterUncreatableMetaObject(paddock::ControllerModel::staticMetaObject,
                                     "Paddock", 1, 0, "ControllerModel",
                                     "Not creatable in QML");
}
} // namespace paddock
