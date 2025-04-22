#include "listwidegt.h"
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QSpacerItem>

ListWidegt::ListWidegt(QWidget* parent):QWidget(parent), pluginListWidget(new QWidget(this)), listModel(new QStringListModel(this)), stackedWidget(new QStackedWidget(this)) {
    setupUI();
}

ListWidegt::ListWidegt(PluginsManager *pluginsManager,QWidget* parent):QWidget(parent), pluginListWidget(new QWidget(this)), listModel(new QStringListModel(this)), stackedWidget(new QStackedWidget(this)){
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
    QVBoxLayout* pluginListLayout = new QVBoxLayout(pluginListWidget);
    // 设置布局的边距和间距为 0，实现顶格显示
    pluginListLayout->setContentsMargins(0, 0, 0, 0);
    pluginListLayout->setSpacing(0);
    pluginListWidget->setLayout(pluginListLayout);
    pluginListWidget->setMaximumWidth(300); // 设置最大宽度
    pluginListWidget->setMinimumWidth(150); // 设置最小宽度
    splitter->addWidget(pluginListWidget);

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
    auto pluginKV = pluginsManager->getPlugins().find(pluginId);
    auto plugin = pluginKV.value();
    stackedWidget->insertWidget(0,plugin->widget);
    ClickableLabel* label = new ClickableLabel(plugin->name, pluginListWidget);
    label->setStyleSheet(R"(
        ClickableLabel {
            background: #f9f9f9; /* 同步右侧整体背景 */
            color: #333; /* 深灰文字 */
            font-family: 'Segoe UI';
            font-size: 14px;
            border: none;
            border-radius: 4px; /* 同步右侧控件圆角 */
            padding: 4px;
            margin: 4px 0;
            text-align: left;
            border-left: 2px solid transparent; /* 选中指示线占位 */
        }
        ClickableLabel:hover {
            background: #f0f0f0; /* 悬停色接近右侧输入框悬停 */
            border-left: 2px solid #4A90E2; /* 预亮指示线 */
        }
        ClickableLabel[selected="true"] {
            background: #4A90E2; /* 主色填充 */
            color: white;
            border-left: 2px solid #2B6BD8; /* 深色指示线增强层次 */
        }
    )");
    label->setProperty("selected", true);
    QVBoxLayout* pluginListLayout = qobject_cast<QVBoxLayout*>(pluginListWidget->layout());
    pluginListLayout->insertWidget(0,label);
    for (int i = 0; i < pluginListLayout->count(); ++i) {
        ClickableLabel* otherLabel = qobject_cast<ClickableLabel*>(pluginListLayout->itemAt(i)->widget());
        if (otherLabel && otherLabel != label) {
            otherLabel->setProperty("selected", false);
            otherLabel->style()->unpolish(otherLabel);
            otherLabel->style()->polish(otherLabel);
        }
    }

    connect(label, &ClickableLabel::clicked, [this, label, index = stackedWidget->count() - 1]() {
        // 取消其他标签的选中状态
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(pluginListWidget->layout());
        for (int i = 0; i < layout->count(); ++i) {
            ClickableLabel* otherLabel = qobject_cast<ClickableLabel*>(layout->itemAt(i)->widget());
            if (otherLabel && otherLabel != label) {
                otherLabel->setProperty("selected", false);
                otherLabel->style()->unpolish(otherLabel);
                otherLabel->style()->polish(otherLabel);
            }
        }
        // 设置当前标签为选中状态
        label->setProperty("selected", true);
        label->style()->unpolish(label);
        label->style()->polish(label);
        stackedWidget->setCurrentIndex(index);
    });
}
void ListWidegt::removePlugin(QString pluginId){
    auto plugin = pluginsManager->getPlugins().find(pluginId);

    stackedWidget->removeWidget(plugin.value()->widget);

    QVBoxLayout* pluginListLayout = qobject_cast<QVBoxLayout*>(pluginListWidget->layout());
    for (int i = 0; i < pluginListLayout->count(); ++i) {
        ClickableLabel* label = qobject_cast<ClickableLabel*>(pluginListLayout->itemAt(i)->widget());
        if (label && label->text() == plugin.value()->name) {
            pluginListLayout->removeWidget(label);
            delete label;
            break;
        }
    }
}

void ListWidegt::showPlugins() {
    // 清空现有标签
    QVBoxLayout* pluginListLayout = qobject_cast<QVBoxLayout*>(pluginListWidget->layout());
    while (pluginListLayout->count() > 0) {
        QWidget* widget = pluginListLayout->takeAt(0)->widget();
        if (widget) {
            delete widget;
        }
    }

    // 清空现有页面
    while (stackedWidget->count() > 0) {
        QWidget* widget = stackedWidget->widget(0);
        stackedWidget->removeWidget(widget);
    }

    for (const auto& plugin : this->pluginsManager->getPlugins().values()) {
        addPlugin(plugin->id);
    }

    // 添加弹簧填充剩余空间
    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    pluginListLayout->addItem(spacer);

    // 默认选择第一个插件
    if (pluginListLayout->count() > 0) {
        ClickableLabel* firstLabel = qobject_cast<ClickableLabel*>(pluginListLayout->itemAt(0)->widget());
        if (firstLabel) {
            // firstLabel->click();
        }
    }

    connect(pluginsManager,&PluginsManager::loadPluginFinish,this,&ListWidegt::addPlugin);
    connect(pluginsManager,&PluginsManager::unloadPluginFinish,this,&ListWidegt::removePlugin);
}
