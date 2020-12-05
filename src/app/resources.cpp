#include "resources.hpp"

#include "Session.hpp"
#include "pads/KorgPadKontrol.hpp"

#include <QtQml/qqml.h>

namespace paddock
{
void initResources()
{
    qmlRegisterUncreatableType<Session>("Paddock", 1, 0, "Session",
                                        "Not creatable in QML");
    qmlRegisterUncreatableType<KorgPadKontrol>("Paddock", 1, 0,
                                               "KorgPadKontrol",
                                               "Not creatable in QML");
    qmlRegisterUncreatableMetaObject(paddock::ControllerModel::staticMetaObject,
                                     "Paddock", 1, 0, "ControllerModel",
                                     "Not creatable in QML");
}
} // namespace paddock
