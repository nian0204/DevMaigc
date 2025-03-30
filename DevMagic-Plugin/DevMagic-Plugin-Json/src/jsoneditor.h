#ifndef JSONEDITOR_H
#define JSONEDITOR_H

#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QPainter>
#include <QJsonParseError>

class JsonEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    JsonEditor(QWidget *parent = nullptr);
    void SetPainText(QString strText);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    // 标记错误
    void markError(const QJsonParseError &error) {
        if (error.error == QJsonParseError::NoError) {
            clearError();
            return;
        }

        int line = getLineNumber(error.offset);
        int column = getColumnNumber(error.offset);

        // 定位到错误位置
        setCursorPosition(line, column);

        // 在文档末尾追加错误提示
        appendPlainText(QString("/* 错误：第%1行，第%2列：%3 */")
                   .arg(line)
                   .arg(column + 1)
                   .arg(error.errorString()));
    }
    // 清除错误标记
    void clearError() {
        QTextCursor cursor(document());
        cursor.movePosition(QTextCursor::End);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deletePreviousChar(); // 删除多余的换行符
    }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    // void paintEvent(QPaintEvent *event) override {
    //     // 先调用父类的 paintEvent 绘制文本内容
    //     QPlainTextEdit::paintEvent(event);

    //     // 绘制行号区域
    //     QPainter painter(viewport());
    //     painter.setPen(Qt::darkGray);

    //     QTextBlock block = document()->firstBlock();
    //     int lineNumber = 1;
    //     while (block.isValid()) {
    //         QRect rect = calculateBlockRect(block);
    //         if (!rect.intersects(event->rect())) {
    //             block = block.next();
    //             continue;
    //         }

    //         // 在行号区域绘制行号
    //         QString number = QString::number(lineNumber);
    //         painter.drawText(0, rect.top(), viewport()->width(), fontMetrics().height(),
    //                          Qt::AlignRight | Qt::AlignVCenter, number);

    //         block = block.next();
    //         lineNumber++;
    //     }
    // }

private slots:
    void slotBlockCountChanged(int newBlockCount);
    void slotUpdateRequest(const QRect &rect, int dy);
    void slotCursorPositionChanged();

private:
    QWidget *lineNumberArea;
    bool m_bInit = false;
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    // 替代 blockBoundingGeometry 的方法
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

    // 获取行号（从1开始，基于文本位置）
    int getLineNumber(int position) const {
        if (position < 0) return 0;
        QTextBlock block = document()->findBlock(position);
        return block.isValid() ? block.blockNumber() + 1 : 0;
    }

    // 获取列号（从0开始，基于文本位置）
    int getColumnNumber(int position) const {
        QTextBlock block = document()->findBlock(position);
        return block.isValid() ? (position - block.position()) : 0;
    }

    // 定位到行列（兼容无光标场景）
    void setCursorPosition(int line, int column) {
        QTextBlock block = document()->findBlockByNumber(line - 1);
        if (!block.isValid()) return;

        int pos = block.position() + column;
        QTextCursor cursor(document());
        cursor.setPosition(pos);
        setTextCursor(cursor);
        ensureCursorVisible();
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


// #ifndef CODEEDITOR_H
// #define CODEEDITOR_H

// #include <QPlainTextEdit>


// //位置：左边LineNumberArea+右边QPlainTextEdit

// class CodeEditor : public QPlainTextEdit
// {
//     Q_OBJECT

// public:
//     CodeEditor(QWidget *parent = nullptr);

//     void SetPainText(QString strText);

//     void lineNumberAreaPaintEvent(QPaintEvent *event);
//     int lineNumberAreaWidth();

// protected:
//     void resizeEvent(QResizeEvent *event) override;

// private slots:
//     void slotBlockCountChanged(int newBlockCount);
//     void slotUpdateRequest(const QRect &rect, int dy);
//     void slotCursorPositionChanged();

// private:
//     void updateLineNumberAreaWidth(int newBlockCount);
//     void highlightCurrentLine();
//     void updateLineNumberArea(const QRect &rect, int dy);


// private:
//     QWidget *lineNumberArea;

//     bool m_bInit = false;
// };

// #endif // CODEEDITOR_H
