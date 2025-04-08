#ifndef LISTWIDEGT_H
#define LISTWIDEGT_H
#include<QWidget>
#include<QListView>
#include<QStringListModel>
#include<QStackedWidget>
#include<QSplitter>
#include<QToolBar>
#include "pluginsmanager.h"
class ListWidegt:public QWidget{
    Q_OBJECT
public:
    explicit ListWidegt(QWidget* parent = nullptr);
    explicit ListWidegt(PluginsManager *pluginsManager,QWidget* parent = nullptr);

    void setPluginsManager(PluginsManager *pluginsManager);
private:
    QListView* listView;
    QStringListModel* listModel;
    QStackedWidget* stackedWidget;
    QSplitter* splitter;

    PluginsManager *pluginsManager;

    void setupUI();
    void showPlugins();

};

#endif // LISTWIDEGT_H

// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

// #include <QMainWindow>
// #include <QListView>
// #include <QStringListModel>
// #include <QStackedWidget>
// #include <QSplitter>
// #include "devplugin.h"
// #include "pluginsmanager.h"
// class MainWindow : public QMainWindow {
//     Q_OBJECT

// public:
//     explicit MainWindow(QWidget* parent = nullptr);
//     void setPlugins(PluginsManager * manager);

// private:
//     QListView* listView;
//     QStringListModel* listModel;
//     QStackedWidget* stackedWidget;
//     QSplitter* splitter;
//     QToolBar* createToolBar();
//     PluginsManager *pluginsManager;
//     void onSettings();
//     void onPlugin();

//     void setupUI();
// };

// #endif // MAINWINDOW_H
