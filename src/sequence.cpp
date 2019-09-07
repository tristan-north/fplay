#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QDateTime>
#include "sequence.h"
#include "mainwindow.h"
#include "common.h"

Sequence::Sequence(QWidget *parent) : QWidget(parent)
{
    setFixedSize(150, 30);

    m_title = QDateTime::currentDateTime().toString("hh:mm:ss");
}


void Sequence::appendFrame(Frame *frame)
{
    m_frames.append(frame);

    MainWindow *mainWindow = MainWindow::getInstance();
    mainWindow->showFrame(frame);
}

int Sequence::getNumFrames()
{
    return m_frames.count();
}


Frame *Sequence::getFrameByFrameNum(int frameNum)
{
    int firstFrameNum = m_frames[0]->m_frameNum;
    int frameIdx = frameNum - firstFrameNum;

    return getFrameByIndex(frameIdx);
}

Frame *Sequence::getFrameByIndex(int frameIdx)
{
    if(frameIdx >= m_frames.count() || frameIdx < 0)
        return nullptr;
    else
        return m_frames[frameIdx];
}

Frame *Sequence::getLastFrame()
{
    return m_frames.last();
}

void Sequence::deleteSeq()
{
    QVectorIterator<Frame*> itFrames(m_frames);
    while (itFrames.hasNext()) {
        delete itFrames.next();
    }

    delete this;
}


void Sequence::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    MainWindow *mainWin = MainWindow::getInstance();

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();

    // Draw bkg
    int bkgColor = 50;
    if(mainWin->getPlayingSequence() == this)
        bkgColor = 60;

    painter.fillRect(0, 0, rect().width()-1, rect().height()-1, QColor(bkgColor, bkgColor, bkgColor));


    // Draw text
    if(mainWin->getPlayingSequence() == this)
        painter.setPen(QColor(200,200,200));
    else
        painter.setPen(QColor(150,150,150));
    painter.drawText(10, rect().height()/2 + fontMetrics.height()/2 -3, m_title);

    // Draw line under widget
    painter.setPen(QColor(20,20,20));
    painter.drawLine(0, rect().height()-1, rect().width(), rect().height()-1);
}

void Sequence::mousePressEvent(QMouseEvent *event)
{
    MainWindow *mainWin = MainWindow::getInstance();

    if( event->button() == Qt::LeftButton ) {
        mainWin->setPlayingSequence(this);
        Frame *frame = getFrameByFrameNum(mainWin->m_currentFrameNum);

        if(frame != nullptr)
            mainWin->showFrame(frame);
    }
}
