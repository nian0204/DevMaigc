#include "pluginmanagerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QPushButton>
#include <QTemporaryDir>
#include <QHeaderView>
#include <QCheckBox>

PluginManagerDialog::PluginManagerDialog(PluginsManager* plugins, QWidget* parent)
    : QDialog(parent), pluginsManager(plugins), networkManager(new QNetworkAccessManager(this)) {
    setWindowTitle(tr("管理插件"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 设置窗口样式（以第二个版本的按钮颜色为准）
    setStyleSheet(R"(
        QWidget {
            font-family: 'Segoe UI';
            font-size: 14px;
        }
        QTableWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            background: #f9f9f9;
            padding: 8px;
        }
        QPushButton {
            padding: 6px 12px;
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

    // 插件表格
    m_pluginTable = new QTableWidget(this);
    m_pluginTable->setColumnCount(3);
    m_pluginTable->setHorizontalHeaderLabels({tr(""), tr("插件名称"), tr("操作")});
    m_pluginTable->horizontalHeader()->setStretchLastSection(true);
    m_pluginTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pluginTable->verticalHeader()->setVisible(false);
    m_pluginTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pluginTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    refreshPluginList();
    mainLayout->addWidget(m_pluginTable);

    // 按钮布局
    auto buttonLayout = new QHBoxLayout();

    // 批量操作按钮
    auto enableAllButton = new QPushButton(tr("全部启用"), this);
    connect(enableAllButton, &QPushButton::clicked, this, &PluginManagerDialog::onEnableAll);
    buttonLayout->addWidget(enableAllButton);

    auto disableAllButton = new QPushButton(tr("全部禁用"), this);
    connect(disableAllButton, &QPushButton::clicked, this, &PluginManagerDialog::onDisableAll);
    buttonLayout->addWidget(disableAllButton);

    auto uninstallAllButton = new QPushButton(tr("全部卸载"), this);
    connect(uninstallAllButton, &QPushButton::clicked, this, &PluginManagerDialog::onUninstallAll);
    buttonLayout->addWidget(uninstallAllButton);

    // 添加插件按钮
    addButtonFromUrl = new QPushButton(tr("从URL添加插件"), this);
    connect(addButtonFromUrl, &QPushButton::clicked, this, &PluginManagerDialog::onAddFromUrl);
    buttonLayout->addWidget(addButtonFromUrl);

    addButtonFromLocal = new QPushButton(tr("从本地文件添加插件"), this);
    connect(addButtonFromLocal, &QPushButton::clicked, this, &PluginManagerDialog::onAddFromLocal);
    buttonLayout->addWidget(addButtonFromLocal);

    mainLayout->addLayout(buttonLayout);

    // 确认/取消按钮
    auto buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(tr("确定"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(tr("取消"), QDialogButtonBox::RejectRole);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    // 表格点击事件
    connect(m_pluginTable, &QTableWidget::cellClicked, this, &PluginManagerDialog::onCellClicked);
}
void PluginManagerDialog::refreshPluginList() {
    m_pluginTable->clearContents();
    m_pluginTable->setRowCount(0);

    auto enabledPlugins = pluginsManager->getEnablePlugins();
    auto disabledPlugins = pluginsManager->getDisablePlugins();
    QMap<QString, bool> pluginStates;

    for (const auto& plugin : enabledPlugins) {
        pluginStates[plugin.id] = true;
    }
    for (const auto& plugin : disabledPlugins) {
        pluginStates[plugin.id] = false;
    }

    int row = 0;
    for (auto it = pluginStates.begin(); it != pluginStates.end(); ++it) {
        const QString& pluginId = it.key();
        bool isEnabled = it.value();

        m_pluginTable->insertRow(row);
        updatePluginRow(row, pluginId, isEnabled);
        ++row;
    }
}

void PluginManagerDialog::updatePluginRow(int row, const QString& pluginId, bool isEnabled) {
    // 第一列：启用状态（复选框）
    auto checkBox = new QCheckBox(this);
    checkBox->setChecked(isEnabled);
    connect(checkBox, &QCheckBox::toggled, [this, pluginId](bool checked) {
        if (checked) {
            pluginsManager->enablePlugin(pluginId);
        } else {
            pluginsManager->disablePlugin(pluginId);
        }
        refreshPluginList();
    });
    m_pluginTable->setCellWidget(row, 0, checkBox);

    // 第二列：插件名称
    auto pluginNameItem = new QTableWidgetItem(pluginId);
    pluginNameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_pluginTable->setItem(row, 1, pluginNameItem);

    // 第三列：操作按钮
    auto enableDisableButton = new QPushButton(isEnabled ? tr("禁用") : tr("启用"), this);
    enableDisableButton->setProperty("enable", !isEnabled);
    enableDisableButton->setStyleSheet(isEnabled ? "background: #FFA500; color: white;" : "background: #4A90E2; color: white;");
    connect(enableDisableButton, &QPushButton::clicked, [this, pluginId, enableDisableButton]() {
        if (pluginsManager->getPlugins().contains(pluginId)) {
            pluginsManager->disablePlugin(pluginId);
        } else {
            pluginsManager->enablePlugin(pluginId);
        }
        refreshPluginList();
    });

    auto uninstallButton = new QPushButton(tr("卸载"), this);
    uninstallButton->setProperty("uninstall", true);
    uninstallButton->setStyleSheet("background: #FF4D4D; color: white;");
    connect(uninstallButton, &QPushButton::clicked, [this, pluginId]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认卸载"),
                                                                  tr("确定要卸载插件 %1 吗？").arg(pluginId),
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            pluginsManager->unloadPluginsByIds({pluginId});
            refreshPluginList();
        }
    });

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(enableDisableButton);
    buttonLayout->addWidget(uninstallButton);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    auto buttonWidget = new QWidget(this);
    buttonWidget->setLayout(buttonLayout);
    m_pluginTable->setCellWidget(row, 2, buttonWidget);
}

void PluginManagerDialog::onEnableAll() {
    auto allPlugins = pluginsManager->getPlugins();
    for (auto it = allPlugins.begin(); it != allPlugins.end(); ++it) {
        pluginsManager->enablePlugin(it.key());
    }
    refreshPluginList();
}

void PluginManagerDialog::onDisableAll() {
    auto allPlugins = pluginsManager->getPlugins();
    for (auto it = allPlugins.begin(); it != allPlugins.end(); ++it) {
        pluginsManager->disablePlugin(it.key());
    }
    refreshPluginList();
}

void PluginManagerDialog::onUninstallAll() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认卸载"),
                                                              tr("确定要卸载所有插件吗？"),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        auto allPlugins = pluginsManager->getPlugins();
        QStringList pluginIds;
        for (auto it = allPlugins.begin(); it != allPlugins.end(); ++it) {
            pluginIds.append(it.key());
        }
        pluginsManager->unloadPluginsByIds(pluginIds);
        refreshPluginList();
    }
}

void PluginManagerDialog::onAddFromUrl() {
    bool ok;
    QString url = QInputDialog::getText(this, tr("从URL添加插件"), tr("请输入插件下载地址:"), QLineEdit::Normal, "", &ok);
    if (ok && !url.isEmpty()) {
        QNetworkRequest request(url);
        QNetworkReply* reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            onDownloadFinished(reply);
        });
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

void PluginManagerDialog::onCellClicked(int row, int column) {
    if (column == 0) {
        // 处理复选框点击事件
        auto checkBox = qobject_cast<QCheckBox*>(m_pluginTable->cellWidget(row, column));
        if (checkBox) {
            checkBox->toggle();
        }
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

void PluginManagerDialog::onDownloadFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存插件"), QDir::homePath(), tr("插件文件 (*.dll *.so)"));
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                QMessageBox::information(this, tr("成功"), tr("插件下载成功！"));
                refreshPluginList();
            } else {
                QMessageBox::warning(this, tr("错误"), tr("无法保存插件文件。"));
            }
        }
    } else {
        QMessageBox::warning(this, tr("错误"), tr("下载插件失败：%1").arg(reply->errorString()));
    }
    reply->deleteLater();
}

void PluginManagerDialog::uninstallPlugin(const QString& pluginId) {
    // 禁用插件
    pluginsManager->disablePlugin(pluginId);

    // 删除插件文件（可选逻辑）
    // QString pluginPath = pluginsManager->getPluginPath(pluginId);
    // if (!pluginPath.isEmpty()) {
    //     QFile::remove(pluginPath);
    // }
}

// void PluginManagerDialog::onDownloadFinished(QNetworkReply* reply) {
//     if (reply->error() == QNetworkReply::NoError) {
//         QByteArray data = reply->readAll();
//         QString fileName = QFileInfo(reply->url().path()).fileName();
//         QString tempZipPath = QDir::tempPath() + "/" + fileName;

//         QFile file(tempZipPath);
//         if (file.open(QIODevice::WriteOnly)) {
//             file.write(data);
//             file.close();

//             QString pluginsDir = Application::pluginsPath();
//             if (extractAndValidateZip(tempZipPath, pluginsDir)) {
//                 QMessageBox::information(this, tr("成功"), tr("插件下载并添加成功！"));
//                 refreshPluginList();
//             } else {
//                 QMessageBox::warning(this, tr("错误"), tr("下载的压缩包不包含有效插件。"));
//             }

//             QFile::remove(tempZipPath);
//         } else {
//             QMessageBox::warning(this, tr("错误"), tr("无法保存下载的插件。"));
//         }
//     } else {
//         QMessageBox::warning(this, tr("错误"), tr("下载插件失败：%1").arg(reply->errorString()));
//     }
//     reply->deleteLater();
// }

void PluginManagerDialog::togglePluginEnable(const QString& pluginId) {
    if (!pluginsManager->getPlugins().contains(pluginId)) {
        qWarning() << "插件不存在：" << pluginId;
        return;
    }

    // 检查当前插件的状态
    // bool isEnabled = pluginsManager->isPluginEnabled(pluginId);

    // if (isEnabled) {
    //     // 如果插件已启用，则禁用它
    //     pluginsManager->disablePlugin(pluginId);
    //     qDebug() << "禁用插件：" << pluginId;
    // } else {
    //     // 如果插件已禁用，则启用它
    //     pluginsManager->enablePlugin(pluginId);
    //     qDebug() << "启用插件：" << pluginId;
    // }

    // // 刷新插件列表以更新界面
    // refreshPluginList();
}
