#include "resources.hpp"

#include "module.hpp"

#include <QtGlobal>

void initUiResources()
{
    Q_INIT_RESOURCE(resources);
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
