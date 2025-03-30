#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H


#include <QWidget>
#include <QSize>

#include "jsoneditor.h"

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(JsonEditor *editor) : QWidget(editor), jsonEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(jsonEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {

        jsonEditor->lineNumberAreaPaintEvent(event);
    }

private:
    JsonEditor *jsonEditor;
};

#endif // LINENUMBERAREA_H
