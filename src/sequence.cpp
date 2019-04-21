#include <QPainter>
#include "sequence.h"

Sequence::Sequence(QWidget *parent) : QWidget(parent)
{
    setFixedSize(150, 30);
}


void Sequence::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();

    painter.drawText(10, rect().height()/2 + fontMetrics.height()/2 -3, "test");

    painter.drawRect(rect().x(), rect().y(), rect().width()-1, rect().height()-1);
}
