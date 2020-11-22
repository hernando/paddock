#include "Session.hpp"
#include "resources.hpp"

#include "core/Globals.hpp"
#include "core/resources.hpp"

#include "ui/resources.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlFileSelector>
#include <QQuickView>

#include "core/Log.hpp"

int main(int argc, char** argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    auto& globals = paddock::core::Globals::instance();
    globals.argc = argc;
    globals.argv = argv;

    paddock::initResources();
    paddock::core::initResources();
    paddock::ui::initResources();

    paddock::Session session;

    QQmlApplicationEngine engine;
    new QQmlFileSelector(&engine); // Engine takes owership
    engine.addImportPath(paddock::ui::qmlResourcePath());

    engine.rootContext()->setContextProperty(QStringLiteral("globalSession"),
                                             &session);

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