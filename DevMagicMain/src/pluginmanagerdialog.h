#ifndef PLUGINMANAGERDIALOG_H
#define PLUGINMANAGERDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QWidget>
#include <QFileDialog>
#include "application.h"
#include "pluginsmanager.h"

class PluginManagerDialog : public QDialog {
    Q_OBJECT

public:
    explicit PluginManagerDialog(PluginsManager* plugins, QWidget* parent = nullptr);

    QStringList disabledPlugins() const;

private slots:
    void onAccepted();

    void onAddPlugin();

private:
    PluginsManager *pluginsManager;
    QListWidget* m_pluginList;
};

#endif // PLUGINMANAGERDIALOG_H
