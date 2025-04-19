#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPluginLoader>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDebug>
#include "devplugin.h"
#include <QStyleFactory>
#include "mainwindow.h"
#include "configmanager.h"
#include "pluginsmanager.h"
#include "application.h"




int main(int argc, char *argv[]) {
    QApplication::setStyle(QStyleFactory::create("Material"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion("1.0.0");

    if (!Application::initApplication()) return -1;

    ConfigManager* config = new ConfigManager();

    PluginsManager *pluginsManager = new PluginsManager(Application::pluginsPath(),config);
    pluginsManager->loadPlugins();

    MainWindow mainWindow;
    mainWindow.setPlugins(pluginsManager);
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
