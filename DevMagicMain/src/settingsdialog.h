#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// #include <QDialog>
// #include <QFormLayout>
// #include <QComboBox>
// #include "configmanager.h"
// // 设置窗口
// class SettingsDialog : public QDialog {
//     Q_OBJECT

// public:
//     explicit SettingsDialog(DevConfig *config, QWidget *parent = nullptr)
//         : QDialog(parent), m_config(config) {
//         setWindowTitle(tr("Settings"));
//         setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

//         auto layout = new QFormLayout(this);

//         // 主题选择
//         m_themeCombo = new QComboBox(this);
//         m_themeCombo->addItem(tr("Auto"), "auto");
//         m_themeCombo->addItem(tr("Light"), "light");
//         m_themeCombo->addItem(tr("Dark"), "dark");
//         m_themeCombo->setCurrentIndex(m_themeCombo->findData(m_config->theme()));
//         layout->addRow(tr("Theme:"), m_themeCombo);

//         // 语言选择
//         m_languageCombo = new QComboBox(this);
//         m_languageCombo->addItem(tr("Chinese (Simplified)"), "zh_CN");
//         m_languageCombo->addItem(tr("English"), "en_US");
//         m_languageCombo->setCurrentIndex(m_languageCombo->findData(m_config->language()));
//         layout->addRow(tr("Language:"), m_languageCombo);

//         // 按钮
//         auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
//         connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
//         connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
//         layout->addWidget(buttonBox);
//     }

// private slots:
//     void onAccepted() {
//         // 更新主题
//         QString selectedTheme = m_themeCombo->currentData().toString();
//         if (selectedTheme != m_config->theme()) {
//             m_config->setTheme(selectedTheme);
//         }

//         // 更新语言
//         QString selectedLanguage = m_languageCombo->currentData().toString();
//         if (selectedLanguage != m_config->language()) {
//             m_config->setLanguage(selectedLanguage);
//         }

//         accept();
//     }

// private:
//     DevConfig *m_config;
//     QComboBox *m_themeCombo;
//     QComboBox *m_languageCombo;
// };
#endif // SETTINGSDIALOG_H
