#include "resources.hpp"

#include "Session.hpp"
#include "pads/KorgPadKontrol.hpp"
#include "pads/korgPadKontrol/Program.hpp"
#include "pads/korgPadKontrol/KnobController.hpp"
#include "pads/korgPadKontrol/KnobModel.hpp"
#include "pads/korgPadKontrol/TriggerController.hpp"
#include "pads/korgPadKontrol/TriggerModel.hpp"

#include <QtQml/qqml.h>

Q_DECLARE_METATYPE(std::error_code);

struct QmlModule
{
    QmlModule(const char* name)
        : _moduleName(name)
    {
    }

    template <typename T>
    void registerType(const char* name)
    {
        qmlRegisterType<T>(_moduleName, 1, 0, name);
    }

    template <typename T>
    void registerUncreatableType(const char* name)
    {
        qmlRegisterUncreatableType<T>(_moduleName, 1, 0, name,
                                      "Not creatable in QML");
    }

    void registerUncreatableMetaObject(const QMetaObject& type,
                                       const char* name)
    {
        qmlRegisterUncreatableMetaObject(type, _moduleName, 1, 0, name,
                                         "Not creatable in QML");
    }

private:
    const char* _moduleName;
};

namespace paddock
{
void registerKorgPadKontrolTypes()
{
    QmlModule module("Paddock.Pads.KorgPadKontrol");
    using namespace korgPadKontrol;

    module.registerUncreatableType<korgPadKontrol::Program>("Program");
    module.registerType<KnobController>("KnobController");
    module.registerType<KnobModel>("KnobModel");
    module.registerType<TriggerController>("TriggerController");
    module.registerType<TriggerModel>("TriggerModel");
}

void initResources()
{
    QmlModule module("Paddock");
    module.registerUncreatableType<Program>("Program");
    module.registerUncreatableType<Session>("Session");

    module.registerUncreatableType<KorgPadKontrol>("KorgPadKontrol");
    module.registerUncreatableMetaObject(ControllerModel::staticMetaObject,
                                         "ControllerModel");
    qRegisterMetaType<std::error_code>();

    registerKorgPadKontrolTypes();
}
} // namespace paddock
