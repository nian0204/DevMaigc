#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QStringList>
#include <QMap>
#include <QPluginLoader>
#include "devplugin.h"
#include "application.h"
#include "configmanager.h"

const QStringList FILTERS = []()->QStringList{
    QStringList filters = QStringList();
#ifdef Q_OS_WIN
    filters << "*.dll";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    filters << "*.so";
#endif
    return filters;
}();

class PluginsManager : public QObject {
    Q_OBJECT
private:
    QString pluginsPath;
    QMap<QString, DevToolPlugin *> plugins;
    ConfigManager *configManager;

public:
    PluginsManager(const QString &path, ConfigManager *configManager);
    void setPluginPath(const QString &path);
    QMap<QString, DevToolPlugin *> getPlugins() const;

    void loadPlugins();
    void unloadPluginsByIds(const QStringList &pluginIds);
    void reLoadPlugins();

    QList<Config::PluginMetaData> getEnablePlugins();
    QList<Config::PluginMetaData> getDisablePlugins();


    void enablePlugin(const QString &pluginId); // 启用插件
    void disablePlugin(const QString &pluginId); // 禁用插件

    void loadPluginsWithoutCheckConfig(QString pluginId);

signals:
    void pluginsAdded(QMap<QString, DevToolPlugin *> &plugins);
    void pluginRemoved(QMap<QString, DevToolPlugin *> &plugins);
    void pluginMapUpdated(const QMap<QString, DevToolPlugin *> &newMap);
    void loadPluginsFinish();
    void loadPluginsError();
    void loadPluginFinish(QString pluginId);
    void unloadPluginFinish(QString pluginId);
};

#endif // PLUGINSMANAGER_H
