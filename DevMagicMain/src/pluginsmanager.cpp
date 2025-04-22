#include "pluginsmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

PluginsManager::PluginsManager(const QString &path, ConfigManager *configManager)
    : pluginsPath(path), configManager(configManager) {}

void PluginsManager::setPluginPath(const QString &path) {
    pluginsPath = path;
}

void PluginsManager::loadPlugins() {
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists()) {
        qWarning() << "插件目录不存在：" << pluginsPath;
        emit loadPluginsError();
        return;
    }

    // 清空当前插件列表
    plugins.clear();

    // 获取启用和禁用的插件列表
    auto enabledPlugins = configManager->getEnabledPlugins();
    auto disabledPlugins = configManager->getDisabledPlugins();

    // 扫描所有插件
    foreach (const QString &pluginId, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString pluginDirPath = pluginsDir.filePath(pluginId);
        QDir pluginDir(pluginDirPath);

        // 查找符合过滤条件的插件文件
        foreach (const QFileInfo &file, pluginDir.entryInfoList(FILTERS)) {
            QPluginLoader loader(file.absoluteFilePath());
            if (loader.load()) {
                if (auto plugin = qobject_cast<DevToolPlugin *>(loader.instance())) {
                    // 初始化插件
                    plugin->init(Application::pluginsPath());
                    const QString toolId = plugin->id; // 插件 ID

                    // 检查插件是否已存在于启用或禁用列表中
                    bool isInEnabledList = false;
                    bool isInDisabledList = false;

                    for (const auto &enabledPlugin : enabledPlugins) {
                        if (enabledPlugin.id == toolId) {
                            isInEnabledList = true;
                            break;
                        }
                    }

                    for (const auto &disabledPlugin : disabledPlugins) {
                        if (disabledPlugin.id == toolId) {
                            isInDisabledList = true;
                            break;
                        }
                    }

                    // 如果插件既不在启用列表也不在禁用列表中，则添加到启用列表
                    if (!isInEnabledList && !isInDisabledList) {
                        Config::PluginMetaData newPluginData;
                        newPluginData.id = toolId;
                        newPluginData.name = plugin->name;
                        enabledPlugins.append(newPluginData);
                        qDebug() << "新插件自动启用 -" << plugin->name << " (" << toolId << ")";
                    }

                    // 将插件加载到内存
                    plugins[toolId] = plugin;
                    qDebug() << "加载插件成功 -" << plugin->name << " (" << toolId << ")";
                } else {
                    qWarning() << "插件格式错误 -" << file.fileName();
                }
            } else {
                qCritical() << "插件加载失败 -" << loader.errorString();
            }
        }
    }

    // 删除配置中不存在的插件
    for (auto it = enabledPlugins.begin(); it != enabledPlugins.end();) {
        if (!plugins.contains(it->id)) {
            it = enabledPlugins.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = disabledPlugins.begin(); it != disabledPlugins.end();) {
        if (!plugins.contains(it->id)) {
            it = disabledPlugins.erase(it);
        } else {
            ++it;
        }
    }

    // 更新配置
    configManager->setEnabledPlugins(enabledPlugins); // 保存启用列表
    configManager->setDisabledPlugins(disabledPlugins); // 保存禁用列表
    configManager->saveConfig();

    // 卸载禁用的插件
    for (const auto &plugin : disabledPlugins) {
        if (plugins.contains(plugin.id)) {
            auto instance = plugins.take(plugin.id);
            delete instance;
            qDebug() << "卸载禁用插件 -" << plugin.name << " (" << plugin.id << ")";
        }
    }

    emit loadPluginsFinish();
}

void PluginsManager::loadPluginsWithoutCheckConfig(QString pluginId) {
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists()) {
        qWarning() << "插件目录不存在：" << pluginsPath;
        emit loadPluginsError();
        return;
    }

    // 扫描所有插件
    foreach (const QString &pluginId, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString pluginDirPath = pluginsDir.filePath(pluginId);
        QDir pluginDir(pluginDirPath);

        // 查找符合过滤条件的插件文件
        foreach (const QFileInfo &file, pluginDir.entryInfoList(FILTERS)) {
            QPluginLoader loader(file.absoluteFilePath());
            if (loader.load()) {
                if (auto plugin = qobject_cast<DevToolPlugin *>(loader.instance())) {
                    // 初始化插件
                    plugin->init(Application::pluginsPath());
                    const QString toolId = plugin->id; // 插件 ID
                    if(toolId==pluginId){
                        // 将插件加载到内存
                        plugins[toolId] = plugin;
                        qDebug() << "加载插件成功 -" << plugin->name << " (" << toolId << ")";
                    }else{
                        delete plugin;
                    }
                } else {
                    qWarning() << "插件格式错误 -" << file.fileName();
                    return;
                }
            } else {
                qCritical() << "插件加载失败 -" << loader.errorString();
                return;
            }
        }
    }
    emit loadPluginFinish(pluginId);
}

QMap<QString, DevToolPlugin *> PluginsManager::getPlugins() const {
    return plugins;
}

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

void PluginsManager::reLoadPlugins() {
    plugins.clear(); // 清空当前插件
    loadPlugins();   // 重新加载插件
}

QList<Config::PluginMetaData> PluginsManager::getEnablePlugins() {
    return configManager->getEnabledPlugins();
}

QList<Config::PluginMetaData> PluginsManager::getDisablePlugins() {
    return configManager->getDisabledPlugins();
}
void PluginsManager::enablePlugin(const QString &pluginId) {
    if (plugins.contains(pluginId)) {
        // 插件已加载，直接更新配置
        configManager->moveToEnabled(pluginId);
        qDebug() << "插件已启用 -" << pluginId;
    } else {
        // 插件未加载，尝试从文件系统加载
        loadPluginsWithoutCheckConfig(pluginId);
        configManager->moveToEnabled(pluginId); // 更新配置
    }
}

void PluginsManager::disablePlugin(const QString &pluginId) {
    if (plugins.contains(pluginId)) {
        // 更新配置
        configManager->moveToDisabled(pluginId);
        //先更新ui
        emit unloadPluginFinish(pluginId);
        qDebug() << "插件已卸载 -" << pluginId;
        // 卸载插件实例
        auto plugin = plugins.take(pluginId);
        delete plugin;
        qDebug() << "插件已禁用 -" << pluginId;

    } else {
        // 插件未加载，仅更新配置
        configManager->moveToDisabled(pluginId);
        qDebug() << "插件已标记为禁用 -" << pluginId;
    }
}
