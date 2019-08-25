#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "sequence.h"
#include "mainwindow.h"

Sequence::Sequence(QWidget *parent) : QWidget(parent)
{
    setFixedSize(150, 30);
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


void Sequence::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();

    painter.drawText(10, rect().height()/2 + fontMetrics.height()/2 -3, "test");

    painter.drawRect(rect().x(), rect().y(), rect().width()-1, rect().height()-1);
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
