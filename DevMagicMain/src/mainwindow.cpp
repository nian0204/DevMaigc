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
#include "PluginManagedTabWidget.h"

MainWindow::MainWindow(PluginsManager * manager,QWidget* parent)
    : QMainWindow(parent) {
    this->pluginsManager=manager;
    setupUI();
}

void MainWindow::setPlugins(PluginsManager* manager) {
    this->pluginsManager = manager;

    pluginTabWidget->setPluginManager(this->pluginsManager);

    // 默认选择第一个插件
    if (pluginTabWidget->count() > 0) {
        pluginTabWidget->setCurrentIndex(0);
    }
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);

    // 创建 PluginManagedTabWidget
    pluginTabWidget = new PluginManagedTabWidget(this->pluginsManager, this);
    pluginTabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            background: #f5f5f5; /* 右侧背景颜色 */
            border: none;
            border-radius: 4px;
        }
        QTabBar::tab {
            background: #f9f9f9; /* Tab 背景颜色 */
            color: #333; /* 深灰文字 */
            font-family: 'Segoe UI';
            font-size: 14px;
            padding: 8px 16px;
            margin-right: 4px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            border: none;
        }
        QTabBar::tab:selected {
            background: #4A90E2; /* 主色填充 */
            color: white;
        }
        QTabBar::tab:hover {
            background: #f0f0f0; /* 悬停色接近右侧输入框悬停 */
        }
    )");

    // 设置主窗口布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(pluginTabWidget);

    setCentralWidget(centralWidget);
    resize(800, 600); // 设置窗口大小

    // 添加顶部工具栏
    addToolBar(createToolBar());
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
        QStringList disabledPlugins = dialog.disabledPlugins();

        // 更新插件列表
        QStringList enabledPlugins;
        for (const auto& plugin : this->pluginsManager->getPlugins().values()) {
            if (!disabledPlugins.contains(plugin->toolId())) {
                enabledPlugins.append(plugin->toolName());
            }
        }

        // 更新 PluginManagedTabWidget
        // pluginTabWidget->updatePluginMap(this->pluginsManager->getPlugins());

    }
}

// 处理设置的操作
void MainWindow::onSettings() {
    qDebug() << "Settings action triggered";
    // 在这里实现具体的逻辑，例如打开设置界面
}
