#include <QLabel>
#include "seqlist.h"
#include "mainwindow.h"


SeqList::SeqList(QWidget *parent) : QWidget(parent)
{
    vboxLayout = new QVBoxLayout();
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->setSpacing(0);
    vboxLayout->addItem(new QSpacerItem(150, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));

//    QHBoxLayout *hboxLayout = new QHBoxLayout();
//    hboxLayout->setContentsMargins(0, 0, 0, 0);
//    hboxLayout->setSpacing(0);
//    hboxLayout->addLayout(vboxLayout);

//    QWidget *seperator = new QWidget();
//    QPalette sepPal;
//    sepPal.setColor(QPalette::Window, QColor(73,73,73));
//    seperator->setPalette(sepPal);
//    seperator->setAutoFillBackground(true);
//    hboxLayout->addWidget(seperator);
//    seperator->setFixedWidth(2);

    setLayout(vboxLayout);
}

Sequence *SeqList::newSequence()
{
    Sequence *newSeq = new Sequence(this);

    vboxLayout->insertWidget(0, newSeq);

    return newSeq;
}

int SeqList::numSequences()
{
    return vboxLayout->count()-1;
}

Sequence *SeqList::getSequenceByIndex(int index)
{
    return static_cast<Sequence*> (vboxLayout->itemAt(index)->widget());
}

int SeqList::getSequenceIndex(Sequence *sequence)
{
    return vboxLayout->indexOf(sequence);
}

QSize SeqList::sizeHint() const
{
    MainWindow *mainWin = MainWindow::getInstance();
    return QSize(150, mainWin->height());
}

