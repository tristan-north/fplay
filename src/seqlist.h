#ifndef SEQLIST_H
#define SEQLIST_H

#include <QFrame>
#include <QLayout>

class SeqList : public QFrame
{
    Q_OBJECT
public:
    explicit SeqList(QWidget *parent = nullptr);
    QVBoxLayout *vboxLayout;

signals:

public slots:
};

#endif // SEQLIST_H
