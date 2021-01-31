#include "Styling.hpp"

#include <QtQml/qqml.h>

#define REGISTER_TYPE(Type) qmlRegisterType<Type>(module, 1, 0, #Type)
#define REGISTER_SINGLETON(Type)    \
    qmlRegisterSingletonType<Type>( \
        "Paddock", 1, 0, #Type,     \
        [](QQmlEngine*, QJSEngine*) -> QObject* { return new Type; })

namespace paddock::ui
{
void registerQmlModule()
{
    REGISTER_SINGLETON(Styling);
}
} // namespace paddock::ui
