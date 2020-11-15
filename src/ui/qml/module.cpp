#include "Styling.hpp"

#include <QtQml/qqml.h>

namespace paddock
{
namespace ui
{
#define REQISTER_TYPE(Type) qmlRegisterType<Type>(module, 1, 0, #Type)
#define REQISTER_SINGLETON(Type)                                             \
    qmlRegisterSingletonType<Type>("Paddock", 1, 0, #Type,                   \
                                   [](QQmlEngine*, QJSEngine*) -> QObject* { \
                                       return new Type                       \
                                   })

void registerQmlModule()
{
    REGISTER_SINGLETON(Styling);
}
} // namespace ui
} // namespace paddock
