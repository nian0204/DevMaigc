#include "pluginmanagedtabwidget.h"
#include <QVBoxLayout>
#include <QSplitter>

PluginManagedTabWidget::PluginManagedTabWidget(PluginsManager *manager, QWidget *parent)
    : QTabWidget(parent), m_pluginManager(manager) {
    initUI();

    // 连接信号与槽
    connect(m_pluginManager, &PluginsManager::pluginsAdded, this, &PluginManagedTabWidget::onPluginsAdded);
    // connect(m_pluginManager, &PluginsManager::pluginsRemoved, this, &PluginManagedTabWidget::onPluginRemoved);
    // connect(m_pluginManager, &PluginsManager::pluginsMapUpdated, this, &PluginManagedTabWidget::onPluginMapUpdated);
}

PluginManagedTabWidget::~PluginManagedTabWidget() {
    // 清理资源
    for (auto it = m_pluginMap.begin(); it != m_pluginMap.end(); ++it) {
        if (!it.value()) {
            delete it.value();
        }
    }
}

void PluginManagedTabWidget::initUI() {
    // 设置可拖动的 Tab 栏和内容区域
    setupTabResizeHandle();
}

void PluginManagedTabWidget::onPluginsAdded(const QMap<QString, DevToolPlugin *> &newMap) {
    addPluginToTabs(newMap);
}

void PluginManagedTabWidget::onPluginsRemoved(const QString &id) {
    removePluginFromTabs(id);
}

void PluginManagedTabWidget::onPluginsMapUpdated(const QMap<QString, DevToolPlugin *> &newMap) {
    // 清除现有的 Tab 和插件
    clear();
    m_pluginMap.clear();

    // 添加新的插件
    for (auto it = newMap.begin(); it != newMap.end(); ++it) {
        m_pluginMap[it.key()] = it.value();
    }

    addPluginToTabs(m_pluginMap);
}

void PluginManagedTabWidget::addPluginToTabs(const QMap<QString, DevToolPlugin *> &newMap) {
    if (newMap.empty()) return;

    // 创建插件的 Widget
    for (auto it = newMap.begin(); it != newMap.end(); ++it) {
        DevToolPlugin * plugin = it.value();
        QWidget *toolWidget = plugin->createToolWidget();
        if (!toolWidget) return;

        // 添加到 TabWidget
        int index = addTab(toolWidget, plugin->toolIcon(), plugin->toolName());

        // 设置 Tab 的工具提示
        setTabToolTip(index, plugin->toolId());

        // 更新映射表
        m_pluginMap[it.key()] = plugin;
    }

}

void PluginManagedTabWidget::removePluginFromTabs(const QString &id) {
    // 查找 Tab 并移除
    for (int i = 0; i < count(); ++i) {
        if (tabToolTip(i) == id) {
            removeTab(i);
            break;
        }
    }

    // 从映射表中移除
    m_pluginMap.remove(id);
}

void PluginManagedTabWidget::setupTabResizeHandle() {
    // 使用 QSplitter 来实现 Tab 栏和内容区域的比例调整
    auto splitter = new QSplitter(Qt::Vertical, this);
    auto tabContainer = new QWidget(splitter);

    QVBoxLayout *layout = new QVBoxLayout(tabContainer);
    layout->addWidget(this);
    layout->setContentsMargins(0, 0, 0, 0);

    splitter->addWidget(tabContainer);
    splitter->setStretchFactor(0, 1); // Tab 栏优先调整
    splitter->setStretchFactor(1, 3); // 内容区域优先调整

    setParent(splitter);
}
