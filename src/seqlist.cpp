#include <QLabel>
#include "seqlist.h"


SeqList::SeqList(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::Box);

    vboxLayout = new QVBoxLayout();
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->setSpacing(0);
    setLayout(vboxLayout);

    vboxLayout->addStretch();
}

Sequence *SeqList::newSequence()
{
    Sequence *newSeq = new Sequence(this);

    // Insert before stretch item at end
    vboxLayout->insertWidget(vboxLayout->count()-1, newSeq);

    return newSeq;
}
