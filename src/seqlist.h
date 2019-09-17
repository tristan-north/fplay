#ifndef SEQLIST_H
#define SEQLIST_H

#include <QFrame>
#include <QLayout>
#include "sequence.h"

class SeqList : public QWidget
{
    Q_OBJECT
public:
    explicit SeqList(QWidget *parent = nullptr);
    Sequence *newSequence();
    int numSequences();
    Sequence *getSequenceByIndex(int index);
    int getSequenceIndex(Sequence *sequence);
    QSize sizeHint() const;

signals:

public slots:

private:
    QVBoxLayout *vboxLayout;
};

#endif // SEQLIST_H
