#ifndef LISTWIDEGT_H
#define LISTWIDEGT_H
#include<QWidget>
#include<QStringListModel>
#include<QStackedWidget>
#include<QSplitter>
#include<QLayout>
#include<QLabel>
#include<QMouseEvent>
#include "pluginsmanager.h"
// 自定义可点击的 QLabel
class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
        QLabel::mousePressEvent(event);
    }
};
class ListWidegt:public QWidget{
    Q_OBJECT
public:
    explicit ListWidegt(QWidget* parent = nullptr);
    explicit ListWidegt(PluginsManager *pluginsManager,QWidget* parent = nullptr);

    void setPluginsManager(PluginsManager *pluginsManager);
public slots:
    void addPlugin(QString pluginId);
    void removePlugin(QString pluginId);

private:
    QWidget* pluginListWidget;
    QStringListModel* listModel;
    QStackedWidget* stackedWidget;
    QSplitter* splitter;

    PluginsManager *pluginsManager;

    void setupUI();
    void showPlugins();

};

#endif
