#ifndef DOCKERWIDGET_H
#define DOCKERWIDGET_H

#include <QWidget>

class DockerWidget : public QWidget
{
    Q_OBJECT

public:
    DockerWidget(QWidget *parent = nullptr);
    ~DockerWidget();
};
#endif // DOCKERWIDGET_H
