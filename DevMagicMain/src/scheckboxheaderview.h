#ifndef SCHECKBOXHEADERVIEW_H
#define SCHECKBOXHEADERVIEW_H
#include <QtGui>
#include <QPainter>
#include <QHeaderView>
#include <QStyleOptionButton>
#include <QStyle>

class SCheckBoxHeaderView : public QHeaderView
{
    Q_OBJECT
private:
    bool isChecked;
    int m_checkColIdx;
public:
    SCheckBoxHeaderView( int checkColumnIndex, Qt::Orientation orientation, QWidget * parent = 0) :
        QHeaderView(orientation, parent) {
        m_checkColIdx = checkColumnIndex;
        isChecked = false;
    }

signals:
    // 修正信号拼写错误
    void checkStatusChange(bool);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {
        painter->save();
        QHeaderView::paintSection(painter, rect, logicalIndex);
        painter->restore();
        if (logicalIndex == m_checkColIdx) {
            QStyleOptionButton option;
            // 计算复选框的尺寸
            QSize checkBoxSize = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option).size();
            // 计算复选框的位置，使其居中
            int x = rect.x() + (rect.width() - checkBoxSize.width()) / 2;
            int y = rect.y() + (rect.height() - checkBoxSize.height()) / 2;
            option.rect = QRect(x, y, checkBoxSize.width(), checkBoxSize.height());
            if (isChecked)
                option.state = QStyle::State_On;
            else
                option.state = QStyle::State_Off;
            this->style()->drawControl(QStyle::CE_CheckBox, &option, painter);
        }
    }
    void mousePressEvent(QMouseEvent *event) {
        if (visualIndexAt(event->pos().x()) == m_checkColIdx) {
            isChecked = !isChecked;
            this->updateSection(m_checkColIdx);
            // 修正信号拼写错误
            emit checkStatusChange(isChecked);
        }
        QHeaderView::mousePressEvent(event);
    }
};
#endif // SCHECKBOXHEADERVIEW_H
