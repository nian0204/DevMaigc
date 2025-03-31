#ifndef JSONPLUGIN_H
#define JSONPLUGIN_H

#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonParseError>
#include <QMap>
#include "jsoneditor.h"
#include "jsonconverter.h"
#include "../include/devplugin.h"

class JsonToolWidget : public QWidget {
    Q_OBJECT
    JsonEditor *inputEditor;
    QTabWidget *outputTabs;
    JsonConverter *converter;
    QMap<QString,QWidget*> tabWidgetMap;

public:
    JsonToolWidget(QWidget *parent = nullptr);
    ~JsonToolWidget() override = default;

private slots:
    void formatJson();
    void escapeJson();
    void compressJson();
    void unescapeJson();
    void validateJson();

private:
    void markError(const QJsonParseError &error);
    void clearError();
    bool isUpdating;
    QWidget* createOutputWidget();
    void createFunctionButton(QBoxLayout *layout, const QString &text, std::function<void()> handler);
    void showParseError(const QJsonParseError &error);
    void setOutputText(const QString &tabName, const QString &text);
    QTextEdit *findTextEdit(QObject *parent);
};

class JsonPlugin : public DevToolPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "forever.landc.devmagic.plugin.json/1.0")
    Q_INTERFACES(DevToolPlugin)

public:
    QString toolId() const override { return "DevMagicPluginJson"; }
    QString toolName() const override { return "JSON工具箱"; }
    QIcon toolIcon() const override { return QIcon("icons/json.svg"); }
    QWidget* createToolWidget() override { return new JsonToolWidget(); }
};
#endif // JSONPLUGIN_H
