#ifndef GITWIDGET_H
#define GITWIDGET_H

#include <QWidget>

class GitWidget : public QWidget
{
    Q_OBJECT

public:
    GitWidget(QWidget *parent = nullptr);
    ~GitWidget();
};
#endif // GITWIDGET_H
