#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#include "timeline.h"
#include "mainwindow.h"

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
}



void Timeline::paintEvent(QPaintEvent *event) {
    MainWindow *mainWin{MainWindow::getInstance()};

    QPainter painter(this);
    QFontMetrics fontMetrics = painter.fontMetrics();
    int horizontalLineYPos = rect().height()/2 + 5;
    //painter.drawRect(rect().x(), rect().y(), rect().width()-1, rect().height()-1);

    // Draw horizonal line across the whole widget
    painter.drawLine(0, horizontalLineYPos, rect().width()-1, horizontalLineYPos);

    int currentFrameNum = mainWin->m_currentFrameNum;
    int firstFrameNum{0}, numFrames{0};
    if(mainWin->currentlyPlayingSeq && mainWin->currentlyPlayingSeq->getFrameByIndex(0)) {
        firstFrameNum = mainWin->currentlyPlayingSeq->getFrameByIndex(0)->m_frameNum;
        numFrames = mainWin->currentlyPlayingSeq->getNumFrames();
    }

    if(numFrames > 1) {
        // Size between tick marks
        float xStepSize = (rect().width()-1) / (numFrames-1.0f);

        // Draw frame ticks
        for(int i=0; i < numFrames; i++) {
            int xPos = static_cast<int>(rect().left() + i*xStepSize);

            painter.drawLine(xPos, horizontalLineYPos+3, xPos, horizontalLineYPos-3);
/*
            // Draw frame num above ticks
            QString frameNumString = QString::number(firstFrameNum+i);
            int stringWidth = fontMetrics.width(frameNumString);

            int frameXPos = xPos;
            if(i==mainWin->getNumFrames()-1)
                frameXPos -= stringWidth;
            else if(i>0)
                frameXPos -= stringWidth/2;

            painter.drawText(frameXPos, rect().height()/2-5, frameNumString);
            */
        }

        // Draw first and last frame number
        QString frameNumString = QString::number(firstFrameNum);
        painter.drawText(0, horizontalLineYPos-5, frameNumString);

        frameNumString = QString::number(firstFrameNum + numFrames-1);
        int stringWidth = fontMetrics.width(frameNumString);
        painter.drawText(rect().width() - stringWidth, horizontalLineYPos-5, frameNumString);


        // Draw current frame box
        int xPos = static_cast<int>((currentFrameNum-firstFrameNum) * xStepSize);
        painter.drawRect(xPos-1, horizontalLineYPos-7, 2, 14);
    }
}

void Timeline::mouseMoveEvent(QMouseEvent *event) {
    MainWindow *mainWin{MainWindow::getInstance()};
    int numFrames = mainWin->currentlyPlayingSeq->getNumFrames();
    if( numFrames < 2 )
        return;
    if( event->buttons() != Qt::LeftButton )
        return;


    float x = static_cast<float>(event->pos().x());

    float spacingBetweenFrameTicks = static_cast<float>(rect().width()) / (numFrames-1);

    int frameIdx = static_cast<int>( x / spacingBetweenFrameTicks + 0.5f);
    Frame *frame = mainWin->currentlyPlayingSeq->getFrameByIndex(frameIdx);
    if(frame)
        mainWin->showFrame(frame);
}

void Timeline::mousePressEvent(QMouseEvent *event) {
    if( event->button() == Qt::LeftButton )
        mouseMoveEvent(event);
}
