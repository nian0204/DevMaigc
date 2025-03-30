#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QStackedWidget>
#include <QSplitter>
#include "devplugin.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void setPlugins(const QList<DevToolPlugin*>& plugins);

private:
    QListView* listView;
    QStringListModel* listModel;
    QStackedWidget* stackedWidget;
    QSplitter* splitter;

    void setupUI();
};

#endif // MAINWINDOW_H
