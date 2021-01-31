#include "resources.hpp"

#include "module.hpp"

#include <QIcon>
#include <QtGlobal>

void initUiResources()
{
    Q_INIT_RESOURCE(resources);
    QIcon::setThemeName("default");
    QIcon::setThemeSearchPaths(QStringList() << ":/icons");
}

namespace paddock::ui
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
} // namespace paddock::ui
