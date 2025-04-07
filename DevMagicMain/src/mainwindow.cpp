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
    : QMainWindow(parent), listView(new QListView(this)), listModel(new QStringListModel(this)), stackedWidget(new QStackedWidget(this)) {
    setupUI();
}

void MainWindow::setPlugins(PluginsManager * manager) {
    this->pluginsManager = manager;
    QStringList pluginNames;

    for (const auto& plugin : this->pluginsManager->getPlugins().values()) {
        pluginNames.append(plugin->toolName()); // 使用插件名称填充列表
        stackedWidget->addWidget(plugin->createToolWidget()); // 创建插件页面并添加到stackedWidget
    }

    listModel->setStringList(pluginNames); // 设置插件名称模型
    listView->setModel(listModel);



    // 美化左侧栏
    listView->setItemDelegate(new QStyledItemDelegate(listView)); // 使用默认样式代理
    listView->setStyleSheet(R"(
        QListView {
            background: #f9f9f9; /* 同步右侧整体背景 */
            color: #333; /* 深灰文字 */
            font-family: 'Segoe UI';
            font-size: 14px;
            border: none;
            border-radius: 4px; /* 同步右侧控件圆角 */
            padding: 4px;
            margin-right: 8px;
        }
        QListView::item {
            border-radius: 4px; /* 统一圆角 */
            margin: 4px 0; /* 增加呼吸感 */
            padding-left: 12px; /* 图标间距 */
            border-left: 2px solid transparent; /* 选中指示线占位 */
        }
        QListView::item:hover {
            background: #f0f0f0; /* 悬停色接近右侧输入框悬停 */
            border-left: 2px solid #4A90E2; /* 预亮指示线 */
        }
        QListView::item:selected {
            background: #4A90E2; /* 主色填充 */
            color: white;
            border-left: 2px solid #2B6BD8; /* 深色指示线增强层次 */
            margin-left: -2px; /* 视觉对齐 */
            padding-left: 10px; /* 修正内边距 */
        }
        QListView::item:selected:hover {
            background: #357AB7; /* 同步按钮悬停色 */
        }
    )");

    // 新增分割线（模拟右侧工具的边框）
    splitter->setStyleSheet("QSplitter::handle { background: #ddd; width: 1px; }");

    // 连接listView的选择信号到stackedWidget的切换槽
    connect(listView->selectionModel(), &QItemSelectionModel::currentChanged,
            [this](const QModelIndex& current, const QModelIndex&) {
                if (current.isValid()) {
                    stackedWidget->setCurrentIndex(current.row());
                }
            });
    // 默认选择第一个插件
    if (!pluginNames.isEmpty()) {
        listView->setCurrentIndex(listModel->index(0));
    }
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);

    // 创建分割器
    splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧插件列表
    listView->setMaximumWidth(300); // 设置最大宽度
    listView->setMinimumWidth(150); // 设置最小宽度
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setVerticalScrollMode(QListView::ScrollPerPixel);
    listView->viewport()->setStyleSheet("background: transparent;"); // 透明滚动区域
    splitter->addWidget(listView);

    // 右侧插件内容区域
    stackedWidget->setStyleSheet("background-color: #f5f5f5;"); // 设置背景颜色
    splitter->addWidget(stackedWidget);

    // 设置分割器初始比例
    splitter->setStretchFactor(0, 1); // 左侧占1份
    splitter->setStretchFactor(1, 3); // 右侧占3份

    // 设置主窗口布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);

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
        listModel->setStringList(enabledPlugins);

        // 清空 stackedWidget
        stackedWidget->clear();

        // 重新添加启用的插件页面
        int currentIndex = -1;
        int index = 0;
        for (const auto& plugin : this->pluginsManager->getPlugins().values()) {
            if (!disabledPlugins.contains(plugin->toolId())) {
                stackedWidget->addWidget(plugin->createToolWidget());

                // 记录第一个启用插件的索引
                if (currentIndex == -1) {
                    currentIndex = index;
                }
                ++index;
            }
        }

        // 默认选择第一个启用的插件
        if (currentIndex != -1) {
            listView->setCurrentIndex(listModel->index(currentIndex));
        }
    }
}

// 处理设置的操作
void MainWindow::onSettings() {
    qDebug() << "Settings action triggered";
    // 在这里实现具体的逻辑，例如打开设置界面
}
