#include "configmanager.h"
#include "application.h"
#include <QCoreApplication>
#include <QDebug>

// Config 类实现
Config::Config() {
    initDefaultValues();
}

void Config::initDefaultValues() {
    theme = "auto";
    language = "zh_CN";
    enabledPlugins.clear();
    disabledPlugins.clear();
}

Config &Config::getInstance() {
    static Config instance; // 单例实例
    return instance;
}

void Config::updateFromJson(const QJsonObject &json) {
    if (json.contains("theme") && json["theme"].isString()) {
        theme = json["theme"].toString();
    }
    if (json.contains("language") && json["language"].isString()) {
        language = json["language"].toString();
    }
    if (json.contains("plugins") && json["plugins"].isObject()) {
        QJsonObject pluginsObj = json["plugins"].toObject();
        if (pluginsObj.contains("enable") && pluginsObj["enable"].isArray()) {
            for (const QJsonValue &value : pluginsObj["enable"].toArray()) {
                if (value.isObject()) {
                    QJsonObject pluginObj = value.toObject();
                    PluginMetaData metaData{
                        pluginObj["name"].toString(),
                        pluginObj["id"].toString(),
                        pluginObj["version"].toString()
                    };
                    enabledPlugins.append(metaData);
                }
            }
        }
        if (pluginsObj.contains("disable") && pluginsObj["disable"].isArray()) {
            for (const QJsonValue &value : pluginsObj["disable"].toArray()) {
                if (value.isObject()) {
                    QJsonObject pluginObj = value.toObject();
                    PluginMetaData metaData{
                        pluginObj["name"].toString(),
                        pluginObj["id"].toString(),
                        pluginObj["version"].toString()
                    };
                    disabledPlugins.append(metaData);
                }
            }
        }
    }
}

// ConfigManager 类实现
ConfigManager::ConfigManager(QObject *parent) :
    QObject(parent),
    m_config(Config::getInstance()) {
    loadConfig(); // 初始化时加载配置文件
}

ConfigManager::~ConfigManager() {
    saveConfig(); // 释放时保存配置文件
}

QString ConfigManager::configFilePath() const {
    return Application::configPath() + "/devmagic_main_config.json";
}

QString ConfigManager::theme() const {
    return m_config.theme;
}

QString ConfigManager::language() const {
    return m_config.language;
}

void ConfigManager::setTheme(const QString &val) {
    if (val == m_config.theme) return;
    m_config.theme = val;
    emit themeChanged();
}

void ConfigManager::setLanguage(const QString &val) {
    if (val == m_config.language) return;
    m_config.language = val;
    emit languageChanged();
}

QList<Config::PluginMetaData> ConfigManager::getEnabledPlugins() const {
    return m_config.enabledPlugins;
}

QList<Config::PluginMetaData> ConfigManager::getDisabledPlugins() const {
    return m_config.disabledPlugins;
}

void ConfigManager::enablePlugin(const Config::PluginMetaData &plugin) {
    auto it = std::find(m_config.disabledPlugins.begin(), m_config.disabledPlugins.end(), plugin);
    if (it != m_config.disabledPlugins.end()) {
        m_config.disabledPlugins.erase(it);
    }

    if (!std::count(m_config.enabledPlugins.begin(), m_config.enabledPlugins.end(), plugin)) {
        m_config.enabledPlugins.append(plugin);
    }
}

void ConfigManager::disablePlugin(const Config::PluginMetaData &plugin) {
    auto it = std::find(m_config.enabledPlugins.begin(), m_config.enabledPlugins.end(), plugin);
    if (it != m_config.enabledPlugins.end()) {
        m_config.enabledPlugins.erase(it);
    }

    if (!std::count(m_config.disabledPlugins.begin(), m_config.disabledPlugins.end(), plugin)) {
        m_config.disabledPlugins.append(plugin);
    }
}
void ConfigManager::moveToDisabled(const QString & pluginId){
    for (auto data = m_config.enabledPlugins.begin();data!=m_config.enabledPlugins.end();data++) {
        if(data->id==pluginId){
            m_config.disabledPlugins.append(*data);
            m_config.enabledPlugins.removeOne(*data);
            this->saveConfig();
            return;
        }
    }
}
void ConfigManager::setEnabledPlugins(QList<Config::PluginMetaData> plugins){
    m_config.enabledPlugins = plugins;
}
void ConfigManager::setDisabledPlugins(QList<Config::PluginMetaData> plugins){
    m_config.disabledPlugins = plugins;
}
void ConfigManager::loadConfig() {
    QFile file(configFilePath());
    if (!file.exists()) {
        qWarning() << "配置文件不存在，使用默认值：" << configFilePath();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "无法读取配置文件：" << configFilePath();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        qCritical() << "配置文件内容无效：" << configFilePath();
        return;
    }

    m_config.updateFromJson(doc.object());
}

void ConfigManager::saveConfig() {
    QFile file(configFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "无法写入配置文件：" << configFilePath();
        return;
    }

    QJsonObject json;
    json["theme"] = m_config.theme;
    json["language"] = m_config.language;

    QJsonObject pluginsObj;
    QJsonArray enabledArray, disabledArray;

    for (const auto &plugin : m_config.enabledPlugins) {
        QJsonObject pluginObj{
            {"name", plugin.name},
            {"id", plugin.id},
            {"version", plugin.version}
        };
        enabledArray.append(pluginObj);
    }

    for (const auto &plugin : m_config.disabledPlugins) {
        QJsonObject pluginObj{
            {"name", plugin.name},
            {"id", plugin.id},
            {"version", plugin.version}
        };
        disabledArray.append(pluginObj);
    }

    pluginsObj["enable"] = enabledArray;
    pluginsObj["disable"] = disabledArray;
    json["plugins"] = pluginsObj;

    file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
}
