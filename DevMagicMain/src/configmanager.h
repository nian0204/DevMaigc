#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include<QString>
#include<QJsonValue>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QDir>
#include<QJsonArray>
#include<QCoreApplication>
#include "application.h"

// 全局配置类
class DevConfig : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit DevConfig(QObject *parent = nullptr);
    QJsonValue getConfigValue(const QString &key, const QJsonValue &defaultVal) const;

    void setConfigValue(const QString &key, const QJsonValue &val);
    void initConfigFile() const;

    QString configFilePath() const;

    QString theme() const;

    QString language() const;

    void setTheme(const QString &val);

    void setLanguage(const QString &val);

signals:
    void themeChanged();
    void languageChanged();

};


#endif // CONFIGMANAGER_H
