#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QStackedWidget>
#include <QSplitter>
#include "devplugin.h"
#include "pluginsmanager.h"
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void setPlugins(PluginsManager * manager);

private:
    QListView* listView;
    QStringListModel* listModel;
    QStackedWidget* stackedWidget;
    QSplitter* splitter;
    QToolBar* createToolBar();
    PluginsManager *pluginsManager;
    void onSettings();
    void onPlugin();

    void setupUI();
};

#endif // MAINWINDOW_H
