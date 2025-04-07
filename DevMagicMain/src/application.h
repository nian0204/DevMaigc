#ifndef APPLICATION_H
#define APPLICATION_H

#include<QString>
#include<QCoreApplication>
#include<QDir>

// 全局常量
const QString APP_NAME = QStringLiteral("DevMagic");
const QString APP_ROOT = []() -> QString {
    QString root = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    if (QDir(root).dirName().toLower() != APP_NAME.toLower()) {
        root = QDir(root).filePath(APP_NAME);
    }
#endif
    return QDir(root).filePath(QStringLiteral(".") + APP_NAME.toLower());
}();
// 应用路径管理工具类
class Application {
public:
    static QString rootPath() { return APP_ROOT; }
    static QString configPath() { return rootPath() + "/config"; }
    static QString pluginsPath() { return rootPath() + "/plugins"; }
    static QString logsPath() { return rootPath() + "/logs"; }

    // 应用初始化
    static bool initApplication() {
        const QStringList dirs = {
            configPath(),
            pluginsPath(),
            logsPath()
        };

        foreach (const QString &dir, dirs) {
            if (!QDir().mkpath(dir)) {
                qCritical() << "无法创建目录 - " << dir;
                return false;
            }
        }

        QFileInfo configInfo(configPath() + "/devmagic_main_config.json");
        if (configInfo.exists() && !configInfo.isWritable()) {
            qCritical() << "配置文件不可写 - " << configInfo.absoluteFilePath();
            return false;
        }

        return true;
    }
};

#endif // APPLICATION_H
