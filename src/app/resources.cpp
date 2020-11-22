#include "resources.hpp"

#include "Session.hpp"

#include <QtQml/qqml.h>

namespace paddock
{
void initResources()
{
    qmlRegisterUncreatableType<Session>("Paddock", 1, 0, "Session",
                                        "Not creatable in QML");
}
} // namespace paddock
