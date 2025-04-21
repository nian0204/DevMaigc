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
public slots:
    void addPlugin(QString pluginId);
    void removePlugin(QString pluginId);

private:
    QListView* listView;
    QStringListModel* listModel;
    QStackedWidget* stackedWidget;
    QSplitter* splitter;

    PluginsManager *pluginsManager;

    void setupUI();
    void showPlugins();

};

#endif
