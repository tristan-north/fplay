#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QValidator>
#include "mainwindow.h"



MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow() : currentlyPlayingSeq(nullptr), currentlyFlippingSeq(nullptr), m_currentFrameNum(-1)
{
    instance = this;
    m_playing = false;

    resize(600,350);
    QHBoxLayout *rootHboxLayout = new QHBoxLayout();
    rootHboxLayout->setContentsMargins(5,0,5,5);
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(rootHboxLayout);

    // Sequence list
    seqList = new SeqList(centralWidget());
//    rootHboxLayout->addLayout(seqList->vboxLayout);
    rootHboxLayout->addWidget(seqList);

    // Image viewport
    QVBoxLayout *vboxLayout = new QVBoxLayout();
    rootHboxLayout->addLayout(vboxLayout);
    vboxLayout->addStretch();
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    vboxLayout->addLayout(hboxLayout);
    hboxLayout->addStretch();
    hboxLayout->addWidget(&m_label);
    hboxLayout->addStretch();
    vboxLayout->addStretch();

    // Play button
    QHBoxLayout *hboxLayoutBot = new QHBoxLayout();
    m_playButton = new QPushButton("Play", centralWidget());
    connect(m_playButton, SIGNAL(released()), this, SLOT(playButtonPushed()));
    hboxLayoutBot->addWidget(m_playButton);
    m_playButton->setFixedSize(90, 30);

    // Current frame text box
    m_currentFrameBox = new QLineEdit(centralWidget());
    m_currentFrameBox->setValidator(new QIntValidator(-99999, 99999, this));
    connect(m_currentFrameBox, SIGNAL(editingFinished()), this, SLOT(currentFrameBoxSet()));
    hboxLayoutBot->addWidget(m_currentFrameBox);
    m_currentFrameBox->setFixedSize(50, 30);

    // Timeline
    m_timeline = new Timeline(centralWidget());
    hboxLayoutBot->addWidget(m_timeline);
    vboxLayout->addLayout(hboxLayoutBot);

}

/*
void MainWindow::showFrame(int frameIdx) {
    if(frameIdx >= m_frames.size() || m_frames.size() == 0 || frameIdx < 0)
        return;

    Frame *frame = m_frames[frameIdx];
    m_label.resize(frame->m_resX, frame->m_resY);
    m_label.setPixmap(frame->m_pixmap);
    m_currentFrameIdx = frameIdx;

    m_timeline->update();
    m_currentFrameBox->setText(QString::number(frame->m_frameNum));
}
*/
void MainWindow::showFrame(Frame *frame) {
    m_label.resize(frame->m_resX, frame->m_resY);
    m_label.setPixmap(frame->m_pixmap);

    m_currentFrameNum = frame->m_frameNum;

    m_timeline->update();
    m_currentFrameBox->setText(QString::number(frame->m_frameNum));
}


void MainWindow::showNextFrame() {
    if(currentlyPlayingSeq->getNumFrames() < 2) {
        if(m_playing)
            QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
        return;
    }

    Frame *frame = currentlyPlayingSeq->getFrameByFrameNum(m_currentFrameNum+1);
    // Frame will be null if the next frame is after the end of the sequence
    if(frame == nullptr)
        frame = currentlyPlayingSeq->getFrameByIndex(0);

    showFrame(frame);

    if(m_playing)
        QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
}


void MainWindow::playButtonPushed() {
    if(m_playing) {
        m_playButton->setText("Play");
        m_playing = false;
    }
    else {
        m_playButton->setText("Pause");
        m_playing = true;
        showNextFrame();
    }
}

void MainWindow::currentFrameBoxSet() {
    if(currentlyPlayingSeq->getNumFrames()==0) {
        m_currentFrameBox->setText("");
        setFocus();
        return;
    }

    int setFrame = m_currentFrameBox->text().toInt();

    Frame *frame = currentlyPlayingSeq->getFrameByFrameNum(setFrame);
    if(frame == nullptr)
        return;

    showFrame(frame);

    setFocus();
}


