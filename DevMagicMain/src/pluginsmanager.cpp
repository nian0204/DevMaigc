#include "pluginsmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// 构造函数
PluginsManager::PluginsManager(const QString &path)
    : pluginsPath(path) {}

// 设置插件路径
void PluginsManager::setPluginPath(const QString &path) {
    pluginsPath = path;
}

// 加载插件
void PluginsManager::loadPlugins() {
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists()) {
        qWarning() << "插件目录不存在：" << pluginsPath;
        emit loadPluginsError();
        return;
    }

    // 遍历每个插件子目录
    foreach (const QString &pluginId, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString pluginDirPath = pluginsDir.filePath(pluginId);
        QDir pluginDir(pluginDirPath);

        // 查找符合过滤条件的插件文件
        foreach (const QFileInfo &file, pluginDir.entryInfoList(FILTERS)) {
            QPluginLoader loader(file.absoluteFilePath());
            if (loader.load()) {
                if (auto plugin = qobject_cast<DevToolPlugin *>(loader.instance())) {
                    plugins[plugin->toolId()] = plugin; // 使用 pluginId 作为键
                    plugin->init(pluginsPath);
                    qDebug() << "加载插件成功 -" << plugin->toolName() << " (" << plugin->toolId() << ")";
                } else {
                    qWarning() << "插件格式错误 -" << file.fileName();
                    emit loadPluginsError();
                }
            } else {
                qCritical() << "插件加载失败 -" << loader.errorString();
                emit loadPluginsError();
            }
        }
    }

    emit loadPluginsFinish();
}

// 获取所有插件
QMap<QString, DevToolPlugin *> PluginsManager::getPlugins() const {
    return plugins;
}

// 根据 pluginIds 加载插件
void PluginsManager::loadPluginsByIds(const QStringList &pluginIds) {
    QMap<QString, DevToolPlugin *> plugins;
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists()) {
        qWarning() << "插件目录不存在：" << pluginsPath;
        emit loadPluginsError();
        return;
    }

    foreach (const QString &pluginId, pluginIds) {
        QString pluginDirPath = pluginsDir.filePath(pluginId);
        QDir pluginDir(pluginDirPath);

        if (!pluginDir.exists()) {
            qWarning() << "插件目录不存在：" << pluginDirPath;
            continue;
        }

        // 查找符合过滤条件的插件文件
        foreach (const QFileInfo &file, pluginDir.entryInfoList(FILTERS)) {
            QPluginLoader loader(file.absoluteFilePath());
            if (loader.load()) {
                if (auto plugin = qobject_cast<DevToolPlugin *>(loader.instance())) {
                    this->plugins[plugin->toolId()] = plugin; // 使用 pluginId 作为键
                    plugins[plugin->toolId()] = plugin;
                    qDebug() << "加载插件成功 -" << plugin->toolName() << " (" << plugin->toolId() << ")";
                } else {
                    qWarning() << "插件格式错误 -" << file.fileName();
                    emit loadPluginsError();
                }
            } else {
                qCritical() << "插件加载失败 -" << loader.errorString();
                emit loadPluginsError();
            }
        }
    }
    emit pluginsAdded(plugins);
}

// 卸载插件
void PluginsManager::unloadPluginsByIds(const QStringList &pluginIds) {
    foreach (const QString &pluginId, pluginIds) {
        if (plugins.contains(pluginId)) {
            auto plugin = plugins.take(pluginId); // 从插件列表中移除
            delete plugin; // 删除插件实例
            qDebug() << "卸载插件成功 -" << pluginId;
        } else {
            qWarning() << "插件未找到 -" << pluginId;
        }
    }
}

// 重新加载插件
void PluginsManager::reLoadPlugins() {
    plugins.clear(); // 清空当前插件
    loadPlugins();   // 重新加载插件
}
