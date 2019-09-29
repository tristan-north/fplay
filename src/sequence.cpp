#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QDateTime>
#include "sequence.h"
#include "mainwindow.h"
#include "common.h"

Sequence::Sequence(QWidget *parent) : QWidget(parent)
{
    setFixedSize(150, 45);

    m_title = QDateTime::currentDateTime().toString("hh:mm:ss");
}


void Sequence::appendFrame(Frame *frame)
{
    m_frames.append(frame);

}

int Sequence::getNumFrames()
{
    return m_frames.count();
}


Frame *Sequence::getFrameByFrameNum(int frameNum)
{
    int firstFrameNum = m_frames[0]->m_frameNum;

    //If frame is out of range return closest frame
    if(frameNum < firstFrameNum) frameNum = firstFrameNum;
    if(frameNum > getLastFrame()->m_frameNum) frameNum = getLastFrame()->m_frameNum;

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
    if(m_frames.data() == nullptr) {
        qInfo() << "m_frames.data() nullptr";
        return nullptr;
    }
    Frame **test = m_frames.data();
    Frame *test2 = *m_frames.data();

    Frame *test3 = m_frames.last();


    return m_frames.last();
}

void Sequence::deleteSeq()
{
    QVectorIterator<Frame*> itFrames(m_frames);
    while (itFrames.hasNext()) {
        delete itFrames.next();
    }

    MainWindow *mainWin = MainWindow::getInstance();

    if(mainWin->getFlippingSequence() == this)
        mainWin->setFlippingSequence(nullptr);
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
        bkgColor = 70;

    painter.fillRect(0, 0, rect().width()-1, rect().height()-1, QColor(bkgColor, bkgColor, bkgColor));

    // Draw icon, aspect ratio of icon area is 1920x1080
    QPixmap *iconPixmap = &getFrameByIndex(0)->m_pixmap;
    int verticalPadding = 5;
    int leftEdgeOffset = 8;
    int iconAreaHeight = rect().height() - verticalPadding*2 - 3;                     // Area not including the frame
    int iconAreaWidth = static_cast<int>(1920.f/1080.f * iconAreaHeight + .5f);       // Area not including the frame

    painter.fillRect(leftEdgeOffset, verticalPadding, iconAreaWidth+2, iconAreaHeight+2, QColor(0, 0, 0));

    int drawHeight = rect().height() - verticalPadding*2 - 3;
    int drawWidth = iconAreaWidth;

    if(static_cast<float>(iconPixmap->width())/iconPixmap->height() > static_cast<float>(drawWidth)/drawHeight) {
        // Pixmap aspect ratio is wider than the area to draw to
        float scaleFactor = static_cast<float>(drawWidth) / iconPixmap->width();
        drawHeight = static_cast<int>(iconPixmap->height()*scaleFactor+0.5f);

        // Ensure there's equal sized black bars on top/bottom
        if(drawHeight % 2 == 1)
            drawHeight++;
    } else {
        // Pixmap aspect ratio is taller than the area to draw to
        float scaleFactor = static_cast<float>(drawHeight) / iconPixmap->height();
        drawWidth = static_cast<int>(iconPixmap->width()*scaleFactor+0.5f);

        // Ensure there's equal sized black bars on left/right
        if(drawWidth % 2 == 0)
            drawWidth++;
    }

    int xOffsetToCenter = (iconAreaWidth - drawWidth) / 2;
    int yOffsetToCenter = (iconAreaHeight - drawHeight) / 2;
    painter.drawPixmap(leftEdgeOffset+1+xOffsetToCenter, verticalPadding+1+yOffsetToCenter, drawWidth, drawHeight, *iconPixmap);


    // Draw text
    if(mainWin->getPlayingSequence() == this)
        painter.setPen(QColor(200,200,200));
    else
        painter.setPen(QColor(150,150,150));
    painter.drawText(75, rect().height()/2 + fontMetrics.height()/2 -3, m_title);


    // Draw line under widget
    painter.setPen(QColor(30,30,30));
    painter.drawLine(0, rect().height()-1, rect().width(), rect().height()-1);
}

void Sequence::mousePressEvent(QMouseEvent *event)
{
    MainWindow *mainWin = MainWindow::getInstance();

    if( event->button() == Qt::LeftButton ) {
        mainWin->setPlayingSequence(this);
    }
}
