#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QStackedWidget>
#include <QSplitter>
#include "devplugin.h"
#include "pluginsmanager.h"
#include "listwidegt.h"
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void setPlugins(PluginsManager * manager);

private:
    ListWidegt *listWidget;
    PluginsManager *pluginsManager;

    QToolBar* createToolBar();

    void onSettings();
    void onPlugin();
};

#endif // MAINWINDOW_H
