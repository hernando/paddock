#include "resources.hpp"

#include "module.hpp"

#include <QtGlobal>

void initUiResources()
{
    Q_INIT_RESOURCE(resources);

#define QML_REGISTER_SINGLETON_TYPE(Type)                                                                              \
    qmlRegisterSingletonType<Type>(moduleName, 1, 0, #Type,                                                            \
                                   [](QQmlEngine*, QJSEngine*) -> QObject* { return new Type(); })

}

namespace paddock
{
namespace ui
{
void initResources()
{
    ::initUiResources();
    registerQmlModule();
}

QString qmlResourcePath()
{
    return "qrc:/paddock/";
}
}
}
