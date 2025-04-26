#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QVector>

class JsonHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    JsonHighlighter(QTextDocument *parent = nullptr);
protected:
    void highlightBlock(const QString &text) override;
private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat booleanFormat;
    QTextCharFormat nullFormat;
};

#endif // JSONHIGHLIGHTER_H
