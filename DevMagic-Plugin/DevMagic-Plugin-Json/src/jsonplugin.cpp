#include "jsonplugin.h"
#include <QMessageBox>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QComboBox>
#include <QLineEdit>
#include "jsonconverter.h"
// 使用 JsonEditor 作为输入编辑器
#include "jsoneditor.h"

JsonToolWidget::JsonToolWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(1200, 800);
    setStyleSheet(R"(
        QWidget {
            font-family: 'Segoe UI';
            font-size: 14px;
        }
        QTextEdit {
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
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // 输入编辑器
    inputEditor = new JsonEditor(this);
    inputEditor->setPlaceholderText("输入JSON或粘贴原始数据...");
    connect(inputEditor, &QPlainTextEdit::textChanged, this, &JsonToolWidget::validateJson); // 实时校验
    mainLayout->addWidget(inputEditor);

    // 功能按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();

    // 格式化、压缩、去转义、添加转义
    createFunctionButton(btnLayout, "格式化", std::bind(&JsonToolWidget::formatJson, this));
    createFunctionButton(btnLayout, "压缩", std::bind(&JsonToolWidget::compressJson, this));
    createFunctionButton(btnLayout, "去转义", std::bind(&JsonToolWidget::unescapeJson, this));
    createFunctionButton(btnLayout, "添加转义", std::bind(&JsonToolWidget::escapeJson, this));

    // 转换语言
    QComboBox *languageSelector = new QComboBox(this);
    languageSelector->addItem("Java");
    languageSelector->addItem("Go");
    languageSelector->addItem("XML");
    btnLayout->addWidget(languageSelector);

    QLineEdit *classNameInput = new QLineEdit(this);
    classNameInput->setPlaceholderText("类名/结构体名");
    classNameInput->setFixedWidth(150);
    btnLayout->addWidget(classNameInput);

    QPushButton *convertBtn = new QPushButton("转换", this);
    connect(convertBtn, &QPushButton::clicked, [this, languageSelector, classNameInput]() {
        QString target = languageSelector->currentText();
        QString className = classNameInput->text().isEmpty() ? "Generated" : classNameInput->text();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);
        if (error.error != QJsonParseError::NoError) return;

        if (target == "Java") {
            QString javaCode = converter.toJavaClass(doc.object(), className);
            setOutputText(0, javaCode);
        } else if (target == "Go") {
            QString goCode = converter.toGoStruct(doc.object(), className);
            setOutputText(0, goCode);
        } else if (target == "XML") {
            QString xml = converter.toXml(doc.object());
            setOutputText(0, xml);
        }
    });
    btnLayout->addWidget(convertBtn);

    mainLayout->addLayout(btnLayout);

    // 输出区域
    outputTabs = new QTabWidget(this);
    outputTabs->addTab(createOutputWidget(), "结果");
    mainLayout->addWidget(outputTabs);
}

void JsonToolWidget::createFunctionButton(QBoxLayout *layout, const QString &text, std::function<void()> handler) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setMinimumWidth(100);
    connect(btn, &QPushButton::clicked, this, handler);
    layout->addWidget(btn);
    layout->addSpacing(8);
}

void JsonToolWidget::formatJson() {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        inputEditor->markError(error); // 调用 JsonEditor 的 markError 方法
        return;
    }

    QString formatted = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    inputEditor->SetPainText(formatted);
}

void JsonToolWidget::compressJson() {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        inputEditor->markError(error); // 调用 JsonEditor 的 markError 方法
        return;
    }

    QString compressed = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    inputEditor->SetPainText(compressed);
}
void JsonToolWidget::unescapeJson() {
    QString text = inputEditor->toPlainText();
    text.replace("\\\"", "\"").replace("\\\\", "\\");
    inputEditor->SetPainText(text);
}

void JsonToolWidget::escapeJson() {
    QString text = inputEditor->toPlainText();
    text.replace("\\", "\\\\").replace("\"", "\\\"");
    inputEditor->SetPainText(text);
}

void JsonToolWidget::validateJson() {
    if (isUpdating) return; // 如果正在更新，直接返回

    isUpdating = true; // 设置标志位
    QJsonParseError error;
    QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        inputEditor->markError(error); // 调用 JsonEditor 的 markError 方法
    } else {
        inputEditor->clearError(); // 清除错误标记
    }
    isUpdating = false; // 重置标志位
}
void JsonToolWidget::setOutputText(int tabIndex, const QString &text) {
    static_cast<QTextEdit *>(outputTabs->widget(tabIndex))->setText(text);
}
QWidget *JsonToolWidget::createOutputWidget() {
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QTextEdit *outputEditor = new QTextEdit(widget);
    outputEditor->setReadOnly(true); // 设置为只读模式
    outputEditor->setStyleSheet("QTextEdit { background: #fafafa; }");
    outputEditor->setPlaceholderText("输出将显示在这里...");

    layout->addWidget(outputEditor);
    return widget;
}
#include "jsonplugin.moc"
