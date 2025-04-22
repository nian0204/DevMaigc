#ifndef DEVPLUGIN_H
#define DEVPLUGIN_H

#include <QtCore/QObject>
#include <QtPlugin>
#include <QIcon>

// 前置声明接口
class DevToolPlugin;
Q_DECLARE_INTERFACE(DevToolPlugin, "forever.landc.devmagic.plugin")

class DevToolPlugin : public QObject {
    Q_OBJECT
    Q_INTERFACES(DevToolPlugin)
    Q_PROPERTY(QString toolId READ toolId CONSTANT)
    Q_PROPERTY(QString toolName READ toolName CONSTANT)
    Q_PROPERTY(QString toolVersion READ toolVersion CONSTANT)
    Q_PROPERTY(QIcon toolIcon READ toolIcon CONSTANT)

public:
    virtual ~DevToolPlugin() = default;
    QWidget *widget;
    QString id;
    QString name;
    QString version;
    QIcon icon;
    void init(QString pluginPath){
        this->id=toolId();
        this->name=toolName();
        this->icon=toolIcon();
        this->pluginPath = pluginPath+"/"+this->id;
        widget = createToolWidget();
    }

    // 可选：支持命令行工具
    virtual void runCommand(const QStringList &args) {
        Q_UNUSED(args);
    }
protected:
    QString pluginPath;
    [[nodiscard]] virtual QWidget *createToolWidget() = 0;
    [[nodiscard]] virtual QString toolId() const = 0;
    [[nodiscard]] virtual QString toolName() const = 0;
    [[nodiscard]] virtual QString toolVersion() const = 0;
    [[nodiscard]] virtual QIcon toolIcon() const = 0;

};

#endif // DEVPLUGIN_H
