#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QJsonDocument>

class Config {
    // 私有构造函数和拷贝构造函数，确保单例模式
    Config();
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;

public:
    // 获取单例实例
    static Config &getInstance();

    // 配置属性及其默认值
    QString theme;
    QString language;

    struct PluginMetaData {
        QString name;
        QString id;
        QString version;

        bool operator==(const PluginMetaData &other) const {
            return id == other.id && name == other.name && version == other.version;
        }
    };

    QList<PluginMetaData> enabledPlugins;
    QList<PluginMetaData> disabledPlugins;

    // 更新配置值的方法
    void updateFromJson(const QJsonObject &json);

private:
    // 默认值初始化
    void initDefaultValues();
};

// 全局配置管理类
class ConfigManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    QString configFilePath() const;

    QString theme() const;
    QString language() const;

    void setTheme(const QString &val);
    void setLanguage(const QString &val);

    QList<Config::PluginMetaData> getEnabledPlugins() const;
    QList<Config::PluginMetaData> getDisabledPlugins() const;

    void setEnabledPlugins(QList<Config::PluginMetaData> plugins);
    void setDisabledPlugins(QList<Config::PluginMetaData> plugins);

    void enablePlugin(const Config::PluginMetaData &plugin);
    void disablePlugin(const Config::PluginMetaData &plugin);

    void loadConfig();
    void saveConfig();

signals:
    void themeChanged();
    void languageChanged();

private:
    Config &m_config; // 单例 Config 实例
};

#endif // CONFIGMANAGER_H
