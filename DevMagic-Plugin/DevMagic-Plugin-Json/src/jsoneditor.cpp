#include "jsoneditor.h"
#include <QDebug>
#include <QTextBlock>
#include <QPainter>
#include <QLinearGradient>
#include "linenumberarea.h"
JsonEditor::JsonEditor(QWidget *parent) : QPlainTextEdit(parent) {

    lineNumberArea = new LineNumberArea(this);

    // 统一基础样式
    setStyleSheet(R"(
        QPlainTextEdit {
            font-family: 'Courier New';
            font-size: 18px; /* 同步主字体大小 */
            border: 1px solid #ddd; /* 统一边框 */
            border-radius: 4px; /* 统一圆角 */
            background: #f9f9f9; /* 同步输入背景 */
            selection-background-color: #4A90E2; /* 同步主色选中效果 */
        }
        QPlainTextEdit:focus {
            border-color: #4A90E2; /* 聚焦时边框高亮 */
            outline: none;
        }
    )");

    // 行号区域样式

    connect(this, &JsonEditor::blockCountChanged, this, &JsonEditor::slotBlockCountChanged);
    connect(this, &JsonEditor::updateRequest, this, &JsonEditor::slotUpdateRequest);
    connect(this, &JsonEditor::cursorPositionChanged, this, &JsonEditor::slotCursorPositionChanged);

    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    setLineWrapMode(QPlainTextEdit::NoWrap);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    m_bInit = true;
}
void JsonEditor::SetPainText(QString strText){
    this->blockSignals(true);//处理期间不响应各种事件
    this->setPlainText(strText);
    this->blockSignals(false);
    updateLineNumberAreaWidth(0);//更新行号(位置和显示文字)
}
// 行号区域尺寸调整（匹配内边距）
int JsonEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void JsonEditor::updateLineNumberAreaWidth(int /* newBlockCount */){
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);//宽度变化了，会触发resize，从而设置行号的位置
}

void JsonEditor::updateLineNumberArea(const QRect &rect, int dy){
    if (dy)
        lineNumberArea->scroll(0, dy);//滚动会用到
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    QRect rctmp =viewport()->rect();

    if (rect.contains(viewport()->rect()))//
        updateLineNumberAreaWidth(0);
}

void JsonEditor::resizeEvent(QResizeEvent *e){

    if(!m_bInit)
    {
        return;
    }

    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();

    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));//设置行号位置
}

//换行会先触发slotCursorPositionChanged，再触发这个
void JsonEditor::slotBlockCountChanged(int newBlockCount){

    if(!m_bInit)
    {
        return;
    }

    updateLineNumberAreaWidth(0);

}
//光标闪动，文字变化，等，都会一直触发此
void JsonEditor::slotUpdateRequest(const QRect &rect, int dy){

    if(!m_bInit)
    {
        return;
    }

    updateLineNumberArea(rect, dy);
}

//不在同一行，在同一行，只要位置变都会触发；选中文字时位置也会变
void JsonEditor::slotCursorPositionChanged(){

    if(!m_bInit)
    {
        return;
    }

    highlightCurrentLine();
}
void JsonEditor::keyPressEvent(QKeyEvent *event) {
    QTextCursor cursor = textCursor();

    // 自动补全括号和引号
    if (event->text() == "{" || event->text() == "(" || event->text() == "[") {
        QString openChar = event->text();
        QString closeChar;

        if (openChar == "{") closeChar = "}";
        else if (openChar == "(") closeChar = ")";
        else if (openChar == "[") closeChar = "]";

        cursor.insertText(openChar + closeChar);
        cursor.movePosition(QTextCursor::Left); // 将光标移动到中间
        setTextCursor(cursor);
        return;
    } else if (event->text() == "\"") {
        cursor.insertText("\"\"");
        cursor.movePosition(QTextCursor::Left); // 将光标移动到中间
        setTextCursor(cursor);
        return;
    }

    // 按下回车时自动缩进
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextBlock currentBlock = cursor.block();
        QString currentLine = currentBlock.text();

        // 计算当前行的缩进
        int indent = 0;
        while (indent < currentLine.size() && currentLine[indent].isSpace()) {
            ++indent;
        }

        // 插入换行符和缩进
        cursor.insertText("\n" + currentLine.left(indent));
        return;
    }

    // 默认处理其他按键
    QPlainTextEdit::keyPressEvent(event);
}
//当前行
void JsonEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        // ✅ 正确构造渐变：先定义起点/终点，再设置颜色停止点
        QLinearGradient gradient(0, 0, 1, 0);  // Qt6推荐使用qreal参数构造
        gradient.setColorAt(0, QColor(0xE9, 0xF5, 0xFF));  // 起始颜色
        gradient.setColorAt(1, QColor(0xD5, 0xEB, 0xFF));  // 结束颜色

        selection.format.setBackground(gradient);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

// 行号绘制优化（同步主字体颜色）
void JsonEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), lineNumberArea->palette().base());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        blockNumber++;
    }
}
// 获取行号（从1开始，基于文本位置）
int JsonEditor::getLineNumber(int position) const {
    if (position < 0) return 0;
    QTextBlock block = document()->findBlock(position);
    return block.isValid() ? block.blockNumber() + 1 : 0;
}

// 获取列号（从0开始，基于文本位置）
int JsonEditor::getColumnNumber(int position) const {
    QTextBlock block = document()->findBlock(position);
    return block.isValid() ? (position - block.position()) : 0;
}
void JsonEditor::setCursorPosition(int line, int column) {
    QTextBlock block = document()->findBlockByNumber(line - 1);
    if (!block.isValid()) return;

    int pos = block.position() + column;
    QTextCursor cursor(document());
    cursor.setPosition(pos);
    setTextCursor(cursor);
    ensureCursorVisible();
}
