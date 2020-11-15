#include "ui/resources.hpp"
#include "core/Session.hpp"

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlApplicationEngine>
#include <QQmlFileSelector>

#include <QScreen>

int main(int argc, char** argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    paddock::ui::initResources();

    paddock::core::Session session;

    QQmlApplicationEngine engine;
    new QQmlFileSelector(&engine);  // Engine takes owership
    engine.addImportPath(paddock::ui::qmlResourcePath());

    engine.load(QUrl{"qrc:paddock/Paddock/MainWindow.qml"});
    const auto rootObjects = engine.rootObjects();

    if (rootObjects.isEmpty())
        return -1;

    QQuickWindow* mainWindow = qobject_cast<QQuickWindow*>(rootObjects[0]);
    if (!mainWindow)
        return false;

    mainWindow->show();

    app.exec();
}
