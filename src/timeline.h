#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

class Timeline : public QWidget
{
    Q_OBJECT

public:
    Timeline(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // TIMELINE_H
