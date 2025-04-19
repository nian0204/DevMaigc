#ifndef PLUGINMANAGERDIALOG_H
#define PLUGINMANAGERDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include "pluginsmanager.h"

class PluginManagerDialog : public QDialog {
    Q_OBJECT

public:
    explicit PluginManagerDialog(PluginsManager* plugins, QWidget* parent = nullptr);

private slots:
    void refreshPluginList();
    void onEnableAll();
    void onDisableAll();
    void onUninstallAll();
    void onCellClicked(int row, int column);
    void onAddFromUrl();
    void onAddFromLocal();
    void onDownloadFinished(QNetworkReply* reply);
    void togglePluginEnable(const QString& pluginId);
    void uninstallPlugin(const QString& pluginId);

private:
    bool validatePlugin(const QString& pluginPath);
    bool extractAndValidateZip(const QString& zipPath, const QString& targetDir);
    void updatePluginRow(int row, const QString& pluginId, bool isEnabled);

    PluginsManager* pluginsManager;
    QNetworkAccessManager* networkManager;
    QTableWidget* m_pluginTable;
    QPushButton* addButtonFromUrl;
    QPushButton* addButtonFromLocal;
};

#endif // PLUGINMANAGERDIALOG_H
