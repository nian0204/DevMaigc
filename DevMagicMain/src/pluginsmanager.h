#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QStringList>
#include <QMap>
#include "QPluginLoader"
#include "devplugin.h"
#include "application.h"

const QStringList FILTERS = []()->QStringList{
    QStringList filters = QStringList();
#ifdef Q_OS_WIN
    filters << "*.dll";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    filters << "*.so";
#endif
    return filters;
}();

class PluginsManager:QObject{
    Q_OBJECT
private:
    QString pluginsPath;
    QMap<QString, DevToolPlugin *> plugins;



signals:
    // 插件添加信号
    void pluginsAdded(QMap<QString, DevToolPlugin *> plugins);

    // 插件移除信号
    void pluginRemoved(QMap<QString, DevToolPlugin *> plugins);

    // 插件映射表更新信号
    void pluginMapUpdated(const QMap<QString, DevToolPlugin *> &newMap);

    void loadPluginsFinish();
    void loadPluginsError();

public:
    PluginsManager(const QString &path);
    void setPluginPath(const QString &path);
    QMap<QString, DevToolPlugin *> getPlugins() const;
    void loadPluginsByIds(const QStringList &pluginIds);
    void unloadPluginsByIds(const QStringList &pluginIds);
    // 插件加载逻辑
    void loadPlugins();
    void reLoadPlugins();
};

#endif // PLUGINSMANAGER_H
