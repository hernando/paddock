#include "resources.hpp"

#include "module.hpp"

#include <QtGlobal>
#include <QIcon>

void initUiResources()
{
    Q_INIT_RESOURCE(resources);
    QIcon::setThemeName("default");
    QIcon::setThemeSearchPaths(QStringList() << ":/icons");
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
