#include "resources.hpp"

#include "Session.hpp"
#include "pads/KorgPadKontrol.hpp"
#include "pads/korgPadKontrol/Program.hpp"
#include "pads/korgPadKontrol/TriggerController.hpp"
#include "pads/korgPadKontrol/TriggerModel.hpp"

#include <QtQml/qqml.h>

Q_DECLARE_METATYPE(std::error_code);

namespace paddock
{
void initResources()
{
    qmlRegisterUncreatableType<Program>("Paddock", 1, 0, "Program",
                                        "Not creatable in QML");
    qmlRegisterUncreatableType<Session>("Paddock", 1, 0, "Session",
                                        "Not creatable in QML");
    qmlRegisterUncreatableType<KorgPadKontrol>(
        "Paddock", 1, 0, "KorgPadKontrol", "Not creatable in QML");
    qmlRegisterUncreatableMetaObject(paddock::ControllerModel::staticMetaObject,
                                     "Paddock", 1, 0, "ControllerModel",
                                     "Not creatable in QML");

    qmlRegisterUncreatableType<korgPadKontrol::Program>(
        "Paddock.Pads.KorgPadKontrol", 1, 0, "Program", "Not creatable in QML");
    qmlRegisterType<korgPadKontrol::TriggerController>(
        "Paddock.Pads.KorgPadKontrol", 1, 0, "TriggerController");
    qmlRegisterType<korgPadKontrol::TriggerModel>(
        "Paddock.Pads.KorgPadKontrol", 1, 0, "TriggerModel");

    qRegisterMetaType<std::error_code>();
}
} // namespace paddock
