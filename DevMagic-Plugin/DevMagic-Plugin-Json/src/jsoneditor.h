#ifndef JSONEDITOR_H
#define JSONEDITOR_H

#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QPainter>
#include <QJsonParseError>
#include "expandbutton.h"

class JsonEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    JsonEditor(QWidget *parent = nullptr);
    void SetPainText(QString strText);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    // 获取行号（从1开始，基于文本位置）
    int getLineNumber(int position) const;
    // 获取列号（从0开始，基于文本位置）
    int getColumnNumber(int position) const;
    // 定位到行列（兼容无光标场景）
    void setCursorPosition(int line, int column);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void slotBlockCountChanged(int newBlockCount);
    void slotUpdateRequest(const QRect &rect, int dy);
    void slotCursorPositionChanged();

private:
    QWidget *lineNumberArea;
    QMap<int, ExpandButton*> m_expandButtons;  // 确保这行存在
    bool m_bInit = false;
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void addExpandButton(int line, int position);
    void updateExpandButtonPositions();
    QRect calculateBlockRect(const QTextBlock &block) const {
        QTextCursor cursor(block);
        cursor.movePosition(QTextCursor::EndOfBlock);
        QRect rect = cursorRect(cursor);
        rect.setLeft(0);
        rect.setWidth(0); // 不影响绘制
        return rect;
    }

    // 计算行号区域的宽度
    int calculateLineNumberAreaWidth() const {
        int digits = 1;
        int maxLine = qMax(1, document()->blockCount());
        while (maxLine >= 10) {
            maxLine /= 10;
            ++digits;
        }
        return fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 10; // 额外留一些间距
    }

    // 辅助：获取当前光标的行列（动态场景）
    QPair<int, int> getCursorPosition() const {
        QTextCursor cursor = textCursor();
        if (cursor.isNull()) return {0, 0};

        int line = getLineNumber(cursor.position());
        int column = getColumnNumber(cursor.position());
        return {line, column};
    }
};

#endif // JSONEDITOR_H
