#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QWidget>

class Sequence : public QWidget
{
    Q_OBJECT
public:
    explicit Sequence(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif // SEQUENCE_H
