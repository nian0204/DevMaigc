#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H

#include <QPushButton>

class ExpandButton : public QPushButton {
    Q_OBJECT
public:
    ExpandButton(QWidget *parent = nullptr);
    void setExpanded(bool expanded);
    bool isExpanded() const;

signals:
    void toggled(bool expanded);

private slots:
    void onButtonClicked();

private:
    bool m_expanded;
};

#endif // EXPANDBUTTON_H
