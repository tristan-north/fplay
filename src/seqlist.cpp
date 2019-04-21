#include <QLabel>
#include "seqlist.h"
#include "sequence.h"

SeqList::SeqList(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::Box);

    vboxLayout = new QVBoxLayout();
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->setSpacing(0);
    setLayout(vboxLayout);

    vboxLayout->addWidget(new Sequence(this));
    vboxLayout->addWidget(new Sequence(this));

    vboxLayout->addStretch();
}
