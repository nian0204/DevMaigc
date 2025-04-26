#include "jsonplugin.h"
#include <QMessageBox>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QComboBox>
#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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
    connect(inputEditor, &JsonEditor::textChanged, this, &JsonToolWidget::validateJson); // 实时校验
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

        QString javaCode = converter->convert(target,doc.object(), className);
        setOutputText("结果", javaCode);
    });
    btnLayout->addWidget(convertBtn);

    mainLayout->addLayout(btnLayout);

    // 输出区域
    outputTabs = new QTabWidget(this);
    QWidget* outputWidget =createOutputWidget();
    outputTabs->addTab(outputWidget, "结果");
    mainLayout->addWidget(outputTabs);
    tabWidgetMap.insert("结果",outputWidget);
    converter = new JsonConverter();
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
        markError(error); // 调用 JsonEditor 的 markError 方法
        return;
    }

    QString formatted = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    inputEditor->SetPainText(formatted);
}

void JsonToolWidget::compressJson() {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        markError(error); // 调用 JsonEditor 的 markError 方法
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
    QJsonParseError error;
    QJsonDocument::fromJson(inputEditor->toPlainText().toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        markError(error); // 调用 JsonEditor 的 markError 方法
    }else{
        clearError();
    }

}
void JsonToolWidget::setOutputText(const QString &tabName, const QString &text) {
    static_cast<QTextEdit *>(tabWidgetMap[tabName])->setText(text);
}
QWidget *JsonToolWidget::createOutputWidget() {

    QTextEdit *outputEditor = new QTextEdit();
    outputEditor->setReadOnly(true); // 设置为只读模式
    outputEditor->setStyleSheet("QTextEdit { background: #fafafa; }");
    outputEditor->setPlaceholderText("输出将显示在这里...");

    return outputEditor;
}
void JsonToolWidget::markError(const QJsonParseError &error) {
    if (error.error == QJsonParseError::NoError) {
        return;
    }
    int line = inputEditor->getLineNumber(error.offset);
    int column = inputEditor->getColumnNumber(error.offset);

    setOutputText("结果",
        QString("/* 错误：第%1行，第%2列：%3 */")
            .arg(line)
            .arg(column + 1)
            .arg(error.errorString()));

    return;
}
void JsonToolWidget::clearError() {
    setOutputText("结果",QString(""));

    return;
}
QTextEdit* JsonToolWidget::findTextEdit(QObject *parent) {
    if (!parent) return nullptr;

    // 检查当前对象是否是 QTextEdit
    QTextEdit *textEdit = qobject_cast<QTextEdit *>(parent);
    if (textEdit) {
        return textEdit;
    }

    // 遍历子对象
    for (QObject *child : parent->children()) {
        QTextEdit *result = findTextEdit(child);
        if (result) {
            return result;
        }
    }

    return nullptr;
}
// // 清除错误标记
// void JsonToolWidget::clearError() {
//     QTextCursor cursor(document());
//     cursor.movePosition(QTextCursor::End);
//     cursor.select(QTextCursor::LineUnderCursor);
//     cursor.removeSelectedText();
//     cursor.deletePreviousChar(); // 删除多余的换行符
// }
#include "jsonplugin.moc"
