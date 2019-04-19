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

    // Get current, first and last frames
    int currentFrameNum{};
    Frame *currentFrame = mainWin->getCurrentFrame();
    if(currentFrame != nullptr)
        currentFrameNum = currentFrame->m_frameNum;

    int firstFrameNum{};
    Frame *firstFrame = mainWin->getFirstFrame();
    if(firstFrame != nullptr)
        firstFrameNum = firstFrame->m_frameNum;


    if(mainWin->getNumFrames() > 1) {
        // Size between tick marks
        float xStepSize = (rect().width()-1) / (mainWin->getNumFrames()-1.0f);

        // Draw frame ticks
        for(int i=0; i < mainWin->getNumFrames(); i++) {
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

        frameNumString = QString::number(firstFrameNum + mainWin->getNumFrames()-1);
        int stringWidth = fontMetrics.width(frameNumString);
        painter.drawText(rect().width() - stringWidth, horizontalLineYPos-5, frameNumString);


        // Draw current frame box
        int xPos = static_cast<int>((currentFrameNum-firstFrameNum) * xStepSize);
        painter.drawRect(xPos-1, horizontalLineYPos-7, 2, 14);
    }
}

void Timeline::mouseMoveEvent(QMouseEvent *event) {
    float x = static_cast<float>(event->pos().x());

    float spacingBetweenFrameTicks = static_cast<float>(rect().width()) / (MainWindow::getInstance()->getNumFrames()-1);

    int frameIdx = static_cast<int>( x / spacingBetweenFrameTicks + 0.5f);
    MainWindow::getInstance()->showFrame(frameIdx);
}
