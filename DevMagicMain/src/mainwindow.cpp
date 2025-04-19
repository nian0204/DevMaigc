#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QStyledItemDelegate>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDebug>
#include "pluginmanagerdialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), listWidget(new ListWidegt(this)) {
    setCentralWidget(listWidget);
    resize(800, 600); // 设置窗口大小
    // 添加顶部工具栏
    addToolBar(createToolBar());
}

void MainWindow::setPlugins(PluginsManager * manager) {
    this->pluginsManager = manager;
    this->listWidget->setPluginsManager(this->pluginsManager);
}

// 创建顶部工具栏
QToolBar* MainWindow::createToolBar() {
    QToolBar* toolBar = new QToolBar("Main Toolbar", this);

    // 增加插件操作
    QAction* addAction = new QAction(QIcon(":/icons/add_plugin.png"), tr("Plugins"), this);
    addAction->setStatusTip(tr("Manage your plugins"));
    connect(addAction, &QAction::triggered, this, &MainWindow::onPlugin);
    toolBar->addAction(addAction);

    // 设置操作
    QAction* settingsAction = new QAction(QIcon(":/icons/settings.png"), tr("Settings"), this);
    settingsAction->setStatusTip(tr("Open settings"));
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
    toolBar->addAction(settingsAction);

    return toolBar;
}

// 处理增加插件的操作
void MainWindow::onPlugin() {
    PluginManagerDialog dialog(this->pluginsManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 获取被禁用的插件 ID
        // QStringList disabledPlugins = dialog.disabledPlugins();
    }
}

// 处理设置的操作
void MainWindow::onSettings() {
    qDebug() << "Settings action triggered";
    // 在这里实现具体的逻辑，例如打开设置界面
}
