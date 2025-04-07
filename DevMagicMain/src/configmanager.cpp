#include "configmanager.h"
#include "application.h"

DevConfig::DevConfig(QObject *parent) : QObject(parent) {
    initConfigFile(); // 初始化配置文件
}

QString DevConfig::theme() const {
    return getConfigValue("theme", "auto").toString();
}

QString DevConfig::language() const {
    return getConfigValue("language", "zh_CN").toString();
}

void DevConfig::setTheme(const QString &val) {
    if (val == theme()) return;
    setConfigValue("theme", val);
    emit themeChanged();
}

void DevConfig::setLanguage(const QString &val) {
    if (val == language()) return;
    setConfigValue("language", val);
    emit languageChanged();
}

QJsonValue DevConfig::getConfigValue(const QString &key, const QJsonValue &defaultVal) const {
    QFile file(configFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法读取配置文件：" << configFilePath() << "，使用默认值：" << key;
        return defaultVal;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "配置文件内容无效：" << configFilePath();
        return defaultVal;
    }

    return doc.object().value(key).isNull() ? defaultVal : doc.object().value(key);
}

void DevConfig::setConfigValue(const QString &key, const QJsonValue &val) {
    QFile file(configFilePath());
    if (!file.open(QIODevice::ReadWrite)) {
        qCritical() << "无法写入配置文件：" << configFilePath();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    obj[key] = val;
    file.resize(0); // 清空文件
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

void DevConfig::initConfigFile() const {
    if (QFile::exists(configFilePath())) return;

    QDir().mkpath(Application::configPath());
    QJsonObject defaultConfig{
        {"theme", "auto"},
        {"language", "zh_CN"},
        {"windowState", QJsonArray()}
    };

    QFile file(configFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(defaultConfig).toJson());
    } else {
        qCritical() << "无法创建默认配置文件：" << configFilePath();
    }
}

QString DevConfig::configFilePath() const {
    return Application::configPath() + "/devmagic_main_config.json";
}
