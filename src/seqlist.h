#ifndef SEQLIST_H
#define SEQLIST_H

#include <QFrame>
#include <QLayout>
#include "sequence.h"

class SeqList : public QFrame
{
    Q_OBJECT
public:
    explicit SeqList(QWidget *parent = nullptr);
    Sequence *newSequence();
    QVBoxLayout *vboxLayout;

signals:

public slots:
};

#endif // SEQLIST_H
