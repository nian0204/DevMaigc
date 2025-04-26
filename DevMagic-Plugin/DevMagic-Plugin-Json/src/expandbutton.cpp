#include "expandbutton.h"

ExpandButton::ExpandButton(QWidget *parent) : QPushButton(parent), m_expanded(false) {
    setFixedSize(16, 16);
    setText("+");
    connect(this, &QPushButton::clicked, this, &ExpandButton::onButtonClicked);
}

void ExpandButton::setExpanded(bool expanded) {
    m_expanded = expanded;
    setText(expanded ? "-" : "+");
}

bool ExpandButton::isExpanded() const {
    return m_expanded;
}

void ExpandButton::onButtonClicked() {
    m_expanded = !m_expanded;
    setText(m_expanded ? "-" : "+");
    emit toggled(m_expanded);
}
