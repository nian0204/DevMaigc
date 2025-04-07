#include "pluginmanagerdialog.h"

PluginManagerDialog::PluginManagerDialog(PluginsManager* plugins, QWidget* parent)
    : QDialog(parent), pluginsManager(plugins) {
    setWindowTitle(tr("Manage Plugins"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto layout = new QVBoxLayout(this);

    // 插件列表
    m_pluginList = new QListWidget(this);
    for (const auto& plugin : pluginsManager->getPlugins()) {
        auto item = new QListWidgetItem(plugin->toolName(), m_pluginList);
        item->setData(Qt::UserRole, plugin->toolId()); // 保存插件 ID
        item->setCheckState(Qt::Checked); // 默认启用
    }
    layout->addWidget(m_pluginList);

    // 按钮
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PluginManagerDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);

    // 添加插件按钮
    QPushButton* addPluginButton = new QPushButton(tr("Add Plugin"), this);
    connect(addPluginButton, &QPushButton::clicked, this, &PluginManagerDialog::onAddPlugin);
    layout->addWidget(addPluginButton);
}
QStringList PluginManagerDialog::disabledPlugins() const {
    QStringList disabled;
    for (int i = 0; i < m_pluginList->count(); ++i) {
        auto item = m_pluginList->item(i);
        if (item->checkState() == Qt::Unchecked) {
            disabled.append(item->data(Qt::UserRole).toString());
        }
    }
    return disabled;
}
void PluginManagerDialog::onAccepted() {
    accept();
}

void PluginManagerDialog::onAddPlugin() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select Plugin File"),
                                                    QDir::homePath(),
                                                    "Plugin Files (*.dll *.so)");
    if (!filePath.isEmpty()) {
        qDebug() << "Selected plugin file:" << filePath;

        // 复制插件到插件目录
        QString targetPath = Application::pluginsPath() + "/" + QFileInfo(filePath).fileName();
        if (QFile::copy(filePath, targetPath)) {
            QMessageBox::information(this, tr("Success"), tr("Plugin added successfully!"));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to add plugin."));
        }
    }
}
