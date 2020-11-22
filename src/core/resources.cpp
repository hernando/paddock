#include "resources.hpp"

#include "Program.hpp"

#include <QtQml/qqml.h>

namespace paddock
{
namespace core
{

void initResources()
{
    qmlRegisterUncreatableType<Program>("Paddock", 1, 0, "Program",
                                        "Not creatable in QML");

}

}
}
