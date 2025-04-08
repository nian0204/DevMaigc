#ifndef PLUGINMANAGERDIALOG_H
#define PLUGINMANAGERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTemporaryDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "pluginsmanager.h"
#include "application.h"


class PluginManagerDialog : public QDialog {
    Q_OBJECT

public:
    explicit PluginManagerDialog(PluginsManager* plugins, QWidget* parent = nullptr);

    QStringList disabledPlugins() const;

private slots:
    void onAccepted();
    void refreshPluginList();
    void onAddFromUrl();
    void onAddFromLocal();
    void onDownloadFinished(QNetworkReply* reply);
    void onUninstallPlugin(); // 新增：卸载插件槽函数

private:
    bool validatePlugin(const QString& pluginPath);
    bool extractAndValidateZip(const QString& zipPath, const QString& targetDir);
    void uninstallPlugin(const QString& pluginId); // 新增：卸载插件逻辑

    PluginsManager* pluginsManager;
    QNetworkAccessManager* networkManager;
    QListWidget* m_pluginList;
    QPushButton* addButtonFromUrl;
    QPushButton* addButtonFromLocal;
};

#endif // PLUGINMANAGERDIALOG_H
