#include "pluginmanagerdialog.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QtNetwork/QNetworkRequest>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDebug>

PluginManagerDialog::PluginManagerDialog(PluginsManager* plugins, QWidget* parent)
    : QDialog(parent), pluginsManager(plugins), networkManager(new QNetworkAccessManager(this)) {
    setWindowTitle(tr("管理插件"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 设置窗口样式
    setStyleSheet(R"(
        QWidget {
            font-family: 'Segoe UI';
            font-size: 14px;
        }
        QListWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            background: #f9f9f9;
            padding: 8px;
        }
        QPushButton {
            padding: 8px 16px;
            background: #4A90E2;
            color: white;
            border: none;
            border-radius: 4px;
            margin-right: 8px;
        }
        QPushButton:hover {
            background: #357AB7;
        }
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background: #ffffff;
        }
    )");

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // 插件列表
    m_pluginList = new QListWidget(this);
    refreshPluginList(); // 初始化插件列表
    mainLayout->addWidget(m_pluginList);

    // 按钮布局
    auto buttonLayout = new QHBoxLayout();

    // 添加插件按钮（从 URL）
    addButtonFromUrl = new QPushButton(tr("从URL添加插件"), this);
    connect(addButtonFromUrl, &QPushButton::clicked, this, &PluginManagerDialog::onAddFromUrl);
    buttonLayout->addWidget(addButtonFromUrl);

    // 添加插件按钮（从本地文件）
    addButtonFromLocal = new QPushButton(tr("从本地文件添加插件"), this);
    connect(addButtonFromLocal, &QPushButton::clicked, this, &PluginManagerDialog::onAddFromLocal);
    buttonLayout->addWidget(addButtonFromLocal);

    // 卸载插件按钮
    auto uninstallButton = new QPushButton(tr("卸载插件"), this);
    connect(uninstallButton, &QPushButton::clicked, this, &PluginManagerDialog::onUninstallPlugin);
    buttonLayout->addWidget(uninstallButton);

    mainLayout->addLayout(buttonLayout);

    // 确认/取消按钮
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(tr("确定"), QDialogButtonBox::AcceptRole);  // 替换 "Ok" 为 "确定"
    buttonBox->addButton(tr("取消"), QDialogButtonBox::RejectRole); // 替换 "Cancel" 为 "取消"

    connect(buttonBox, &QDialogButtonBox::accepted, this, &PluginManagerDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
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

void PluginManagerDialog::refreshPluginList() {
    m_pluginList->clear();
    for (const auto& plugin : pluginsManager->getPlugins()) {
        auto item = new QListWidgetItem(plugin->toolName(), m_pluginList);
        item->setData(Qt::UserRole, plugin->toolId()); // 保存插件 ID
        // item->setCheckState(plugin->isEnabled() ? Qt::Checked : Qt::Unchecked); // 根据插件状态设置勾选
    }
}

bool PluginManagerDialog::validatePlugin(const QString& pluginPath) {
    QFileInfo fileInfo(pluginPath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }

    QString suffix = fileInfo.suffix().toLower();
    if (suffix != "dll" && suffix != "so") {
        return false;
    }

    return true;
}

bool PluginManagerDialog::extractAndValidateZip(const QString& zipPath, const QString& targetDir) {
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qWarning() << "Failed to create temporary directory.";
        return false;
    }

    QString command = QString("unzip %1 -d %2").arg(zipPath, tempDir.path());
    int result = QProcess::execute(command);
    if (result != 0) {
        qWarning() << "Failed to extract ZIP file:" << zipPath;
        return false;
    }

    QDir extractedDir(tempDir.path());
    bool isValid = false;
    for (const auto& entry : extractedDir.entryList(QDir::Files)) {
        QString filePath = extractedDir.absoluteFilePath(entry);
        if (validatePlugin(filePath)) {
            isValid = true;
            QString targetPath = targetDir + "/" + QFileInfo(filePath).fileName();
            QFile::copy(filePath, targetPath);
        }
    }

    return isValid;
}

void PluginManagerDialog::onAddFromUrl() {
    bool ok;
    QString url = QInputDialog::getText(this, tr("从URL添加插件"), tr("输入插件URL:"), QLineEdit::Normal, "", &ok);
    if (ok && !url.isEmpty()) {
        QNetworkRequest request(url);
        QNetworkReply* reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onDownloadFinished(reply); });
    }
}

void PluginManagerDialog::onAddFromLocal() {
    QString zipPath = QFileDialog::getOpenFileName(this, tr("选择插件压缩包"),
                                                   QDir::homePath(),
                                                   "ZIP Files (*.zip)");
    if (!zipPath.isEmpty()) {
        QString pluginsDir = Application::pluginsPath();
        if (extractAndValidateZip(zipPath, pluginsDir)) {
            QMessageBox::information(this, tr("成功"), tr("插件添加成功！"));
            refreshPluginList();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("所选压缩包不包含有效插件。"));
        }
    }
}

void PluginManagerDialog::onDownloadFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString fileName = QFileInfo(reply->url().path()).fileName();
        QString tempZipPath = QDir::tempPath() + "/" + fileName;

        QFile file(tempZipPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();

            QString pluginsDir = Application::pluginsPath();
            if (extractAndValidateZip(tempZipPath, pluginsDir)) {
                QMessageBox::information(this, tr("成功"), tr("插件下载并添加成功！"));
                refreshPluginList();
            } else {
                QMessageBox::warning(this, tr("错误"), tr("下载的压缩包不包含有效插件。"));
            }

            QFile::remove(tempZipPath);
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法保存下载的插件。"));
        }
    } else {
        QMessageBox::warning(this, tr("错误"), tr("下载插件失败：%1").arg(reply->errorString()));
    }
    reply->deleteLater();
}

void PluginManagerDialog::onUninstallPlugin() {
    QList<QListWidgetItem*> selectedItems = m_pluginList->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请先选择要卸载的插件！"));
        return;
    }

    for (auto item : selectedItems) {
        QString pluginId = item->data(Qt::UserRole).toString();
        uninstallPlugin(pluginId);
    }

    QMessageBox::information(this, tr("成功"), tr("插件已成功卸载！"));
    refreshPluginList();
}

void PluginManagerDialog::uninstallPlugin(const QString& pluginId) {
    // 禁用插件
    // pluginsManager->disablePlugin(pluginId);

    // 删除插件文件
    // QString pluginPath = pluginsManager->getPluginPath(pluginId);
    // if (!pluginPath.isEmpty()) {
    //     QFile::remove(pluginPath);
    // }
}
