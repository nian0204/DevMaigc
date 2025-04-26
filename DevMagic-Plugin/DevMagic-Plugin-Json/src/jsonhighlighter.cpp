#include "jsonhighlighter.h"
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QVector>
#include <QColor>

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    // 定义关键字格式（花括号、方括号、冒号、逗号等）
    keywordFormat.setForeground(Qt::darkBlue);
    QRegularExpression keywordPattern("\\{|\\}|\\[|\\]|\\:|\\,");
    highlightingRules.append({keywordPattern, keywordFormat});

    // 定义字符串格式
    stringFormat.setForeground(Qt::darkGreen);
    QRegularExpression stringPattern("\".*?\"");
    highlightingRules.append({stringPattern, stringFormat});

    // 定义数字格式
    numberFormat.setForeground(Qt::darkMagenta);
    QRegularExpression numberPattern("\\b\\d+\\b");
    highlightingRules.append({numberPattern, numberFormat});

    // 定义布尔值格式
    booleanFormat.setForeground(Qt::darkRed);
    QRegularExpression booleanPattern("\\b(true|false)\\b");
    highlightingRules.append({booleanPattern, booleanFormat});

    // 定义 null 格式
    nullFormat.setForeground(Qt::darkCyan);
    QRegularExpression nullPattern("\\bnull\\b");
    highlightingRules.append({nullPattern, nullFormat});

    // 定义键格式
    QTextCharFormat keyFormat;
    keyFormat.setForeground(Qt::blue);
    QRegularExpression keyPattern("\"([^\"]+)\"\\s*:\\s*");
    highlightingRules.append({keyPattern, keyFormat});
}

void JsonHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            int index = match.capturedStart();
            int length = match.capturedLength();
            if (rule.pattern.pattern() == "\"([^\"]+)\"\\s*:\\s*") {
                // 对于键，只高亮键部分，不包括冒号和空格
                int keyLength = match.captured(1).length();
                setFormat(index + 1, keyLength, rule.format);
            } else {
                setFormat(index, length, rule.format);
            }
        }
    }
}
