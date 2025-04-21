#include "listwidegt.h"
#include <QStyledItemDelegate>
#include <QVBoxLayout>

ListWidegt::ListWidegt(QWidget* parent):QWidget(parent), listView(new QListView(this)), listModel(new QStringListModel(this)), stackedWidget(new QStackedWidget(this)) {
    setupUI();
}

ListWidegt::ListWidegt(PluginsManager *pluginsManager,QWidget* parent):QWidget(parent), listView(new QListView(this)), listModel(new QStringListModel(this)), stackedWidget(new QStackedWidget(this)){
    this->pluginsManager = pluginsManager;
    setupUI();
    showPlugins();
};
void ListWidegt::setPluginsManager(PluginsManager *pluginsManager){
    this->pluginsManager=pluginsManager;
    showPlugins();
};

void ListWidegt::setupUI(){
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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(splitter);

    resize(800, 600); // 设置窗口大小
};

void ListWidegt::addPlugin(QString pluginId){
    auto plugin = pluginsManager->getPlugins().find(pluginId);

    stackedWidget->addWidget(plugin.value()->widget);

    listModel->insertRow(listModel->rowCount()); //在尾部插入一空行
    //QModelIndex index;
    QModelIndex index=listModel->index(listModel->rowCount()-1,0);//获取最后一行

    listModel->setData(index,plugin.value()->toolName(),Qt::DisplayRole);//设置显示文字
}
void ListWidegt::removePlugin(QString pluginId){
    auto plugin = pluginsManager->getPlugins().find(pluginId);


    stackedWidget->removeWidget(plugin.value()->widget);
    auto index = listModel->stringList().indexOf(plugin.value()->toolName());
    if(index!=-1){
        listModel->removeRow(index);
    }
}
void ListWidegt::showPlugins() {
    QStringList pluginNames;

    for (const auto& plugin : this->pluginsManager->getPlugins().values()) {
        pluginNames.append(plugin->toolName()); // 使用插件名称填充列表
        stackedWidget->addWidget(plugin->widget); // 创建插件页面并添加到stackedWidget
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
    connect(pluginsManager,&PluginsManager::loadPluginFinish,this,&ListWidegt::addPlugin);
    connect(pluginsManager,&PluginsManager::unloadPluginFinish,this,&ListWidegt::removePlugin);
}

