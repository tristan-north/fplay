#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QValidator>
#include <QKeyEvent>
#include "mainwindow.h"
#include "common.h"



MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow() : m_currentFrameNum(-1), m_currentlyPlayingSeq(nullptr), m_currentlyFlippingSeq(nullptr)
{
    instance = this;
    m_playing = false;

    resize(600,350);
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Window, QColor(46,46,46));
    setAutoFillBackground(true);
    setPalette(pal);

    QHBoxLayout *rootHboxLayout = new QHBoxLayout();
    rootHboxLayout->setContentsMargins(0,0,0,0);
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
    hboxLayoutBot->setContentsMargins(0,0,5,5);
    m_playButton = new QPushButton("Play", centralWidget());

    QPalette playPal = QApplication::palette();
    playPal.setColor(QPalette::Button, QColor(50,50,50));
    playPal.setColor(QPalette::ButtonText, QColor(TEXTCOLOR));
    playPal.setColor(QPalette::Window, QColor(30,30,30)); // The button border
    m_playButton->setPalette(playPal);

    connect(m_playButton, SIGNAL(released()), this, SLOT(playButtonPushed()));
    hboxLayoutBot->addWidget(m_playButton);
    m_playButton->setFixedSize(60, 30);

    // Current frame text box
    m_currentFrameBox = new QLineEdit(centralWidget());
    m_currentFrameBox->setValidator(new QIntValidator(-99999, 99999, this));

    QPalette framePal = QApplication::palette();
    framePal.setColor(QPalette::Base, QColor(19,19,19));
    framePal.setColor(QPalette::Window, QColor(19,19,19));  // The border
    framePal.setColor(QPalette::Text, QColor(TEXTCOLOR));
    m_currentFrameBox->setPalette(framePal);

    connect(m_currentFrameBox, SIGNAL(editingFinished()), this, SLOT(currentFrameBoxSet()));
    hboxLayoutBot->addWidget(m_currentFrameBox);
    m_currentFrameBox->setFixedSize(48, 25);

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
    if(frame == nullptr) {
        static QPixmap nullPixmap;
        m_label.setPixmap(nullPixmap);

        m_currentFrameNum = 0;

        m_timeline->update();
        m_currentFrameBox->setText("");
        return;
    }

    m_label.resize(frame->m_resX, frame->m_resY);
    m_label.setPixmap(frame->m_pixmap);

    m_currentFrameNum = frame->m_frameNum;

    m_timeline->update();
    m_currentFrameBox->setText(QString::number(frame->m_frameNum));
}


void MainWindow::showNextFrame() {
    if(!m_currentlyPlayingSeq)
        return;

    if(m_currentlyPlayingSeq->getNumFrames() < 2) {
        if(m_playing)
            QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
        return;
    }

    Frame *frame = m_currentlyPlayingSeq->getFrameByFrameNum(m_currentFrameNum+1);
    // Frame will be null if the next frame is after the end of the sequence
    if(frame == nullptr)
        frame = m_currentlyPlayingSeq->getFrameByIndex(0);

    showFrame(frame);

    if(m_playing)
        QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
}


void MainWindow::playButtonPushed() {
    if(!m_currentlyPlayingSeq)
        return;

    if(m_playing) {
        m_playButton->setText("Play");
        m_playing = false;
    }
    else {
        m_playButton->setText("Stop");
        m_playing = true;
        showNextFrame();
    }
}

void MainWindow::currentFrameBoxSet() {
    if(!m_currentlyPlayingSeq)
        return;

    if(m_currentlyPlayingSeq->getNumFrames()==0) {
        m_currentFrameBox->setText("");
        setFocus();
        return;
    }

    int setFrame = m_currentFrameBox->text().toInt();

    Frame *frame = m_currentlyPlayingSeq->getFrameByFrameNum(setFrame);
    if(frame == nullptr)
        return;

    showFrame(frame);

    setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete) {
        Sequence *seqToDelete = m_currentlyPlayingSeq;

        int currentIndex = seqList->getSequenceIndex(m_currentlyPlayingSeq);

        // If its the last sequence clean things up and stop playing if its currently playing.
        if(seqList->numSequences() == 1) {
            // Stop playing
            if(m_playing)
                playButtonPushed();

            m_currentlyPlayingSeq = nullptr;
            m_currentlyFlippingSeq = nullptr;
            showFrame(nullptr);
        }
        else if(currentIndex == 0)
            setPlayingSequence(seqList->getSequenceByIndex(currentIndex+1));
        else
            setPlayingSequence(seqList->getSequenceByIndex(currentIndex-1));

        seqToDelete->deleteSeq();
    }
}

Sequence *MainWindow::getPlayingSequence()
{
    return m_currentlyPlayingSeq;
}

Sequence *MainWindow::getFlippingSequence()
{
    return m_currentlyFlippingSeq;
}

void MainWindow::setPlayingSequence(Sequence *seq)
{
    m_currentlyPlayingSeq = seq;

    for(int i=0; i<seqList->numSequences(); i++) {
        seqList->getSequenceByIndex(i)->repaint();
    }

}

void MainWindow::setFlippingSequence(Sequence *seq)
{
    m_currentlyFlippingSeq = seq;
}


