#ifndef PLUGINMANAGEDTABWIDGET_H
#define PLUGINMANAGEDTABWIDGET_H

#include <QTabWidget>
#include "pluginsmanager.h"

class PluginManagedTabWidget : public QTabWidget {
    Q_OBJECT

public:
    explicit PluginManagedTabWidget(PluginsManager *manager, QWidget *parent = nullptr);
    ~PluginManagedTabWidget() override;

private slots:
    // 处理插件添加
    void onPluginsAdded(const QMap<QString, DevToolPlugin *> &newMap);

    // 处理插件移除
    void onPluginsRemoved(const QString &id);

    // 处理插件映射表更新
    void onPluginsMapUpdated(const QMap<QString, DevToolPlugin *> &newMap);

private:
    PluginsManager *m_pluginManager;
    QMap<QString, DevToolPlugin*> m_pluginMap;

    // 初始化界面布局
    void initUI();

    // 添加插件到 TabWidget
    void addPluginToTabs(const QMap<QString, DevToolPlugin *> &newMap);

    // 移除插件 Tab
    void removePluginFromTabs(const QString &id);

    void setupTabResizeHandle();
};

#endif // PLUGINMANAGEDTABWIDGET_H
