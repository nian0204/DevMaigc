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
const QStringList FILTERS = []()->QStringList{
    QStringList filters = QStringList();
#ifdef Q_OS_WIN
    filters << "*.dll";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    filters << "*.so";
#endif
    return filters;
}();

// 应用路径管理工具类
class AppPaths {
public:
    static QString rootPath() { return APP_ROOT; }
    static QString configPath() { return rootPath() + "/config"; }
    static QString pluginsPath() { return rootPath() + "/plugins"; }
    static QString logsPath() { return rootPath() + "/logs"; }
};

// 全局配置类
class DevConfig : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit DevConfig(QObject *parent = nullptr) : QObject(parent) {
        initConfigFile(); // 初始化配置文件
    }

    QString theme() const {
        return getConfigValue("theme", "auto").toString();
    }

    QString language() const {
        return getConfigValue("language", "zh_CN").toString();
    }

    void setTheme(const QString &val) {
        if (val == theme()) return;
        setConfigValue("theme", val);
        emit themeChanged();
    }

    void setLanguage(const QString &val) {
        if (val == language()) return;
        setConfigValue("language", val);
        emit languageChanged();
    }

signals:
    void themeChanged();
    void languageChanged();

private:
    QJsonValue getConfigValue(const QString &key, const QJsonValue &defaultVal) const {
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

    void setConfigValue(const QString &key, const QJsonValue &val) {
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

    void initConfigFile() const {
        if (QFile::exists(configFilePath())) return;

        QDir().mkpath(AppPaths::configPath());
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

    QString configFilePath() const {
        return AppPaths::configPath() + "/settings.json";
    }
};

// 插件加载逻辑
void loadPlugins(QList<DevToolPlugin*> &plugins) {
    const QString pluginDir = AppPaths::pluginsPath();
    QDir dir(pluginDir);

    foreach (const QFileInfo &file, dir.entryInfoList(FILTERS)) {
        QPluginLoader loader(file.absoluteFilePath());
        if (loader.load()) {
            if (auto plugin = qobject_cast<DevToolPlugin *>(loader.instance())) {
                plugins.append(plugin);
                qDebug() << "加载插件成功 - " << plugin->toolName();
            } else {
                qWarning() << "插件格式错误 - " << file.fileName();
            }
        } else {
            qCritical() << "插件加载失败 - " << loader.errorString();
        }
    }
}

// 应用初始化
bool initApplication() {
    const QStringList dirs = {
        AppPaths::configPath(),
        AppPaths::pluginsPath(),
        AppPaths::logsPath()
    };

    foreach (const QString &dir, dirs) {
        if (!QDir().mkpath(dir)) {
            qCritical() << "无法创建目录 - " << dir;
            return false;
        }
    }

    QFileInfo configInfo(AppPaths::configPath() + "/settings.json");
    if (configInfo.exists() && !configInfo.isWritable()) {
        qCritical() << "配置文件不可写 - " << configInfo.absoluteFilePath();
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    QApplication::setStyle(QStyleFactory::create("Material"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion("1.0.0");

    if (!initApplication()) return -1;

#ifdef QT_DEBUG
    const QString devPluginPath = QStringLiteral(":/dev-plugins/");
    QDir devDir(devPluginPath);
    devDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    foreach (const QFileInfo &fileInfo, devDir.entryInfoList()) {
        QString sourceFilePath = fileInfo.absoluteFilePath();
        QString targetFilePath = AppPaths::pluginsPath() + "/" + fileInfo.fileName();
        if (!QFile::copy(sourceFilePath, targetFilePath)) {
            qWarning() << "Failed to copy plugin file: " << sourceFilePath;
        }
    }
#endif

    DevConfig config;

    // 加载插件
    QList<DevToolPlugin*> plugins;
    loadPlugins(plugins);

    MainWindow mainWindow;
    mainWindow.setPlugins(plugins);
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
