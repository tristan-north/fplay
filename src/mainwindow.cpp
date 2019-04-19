#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QValidator>
#include "mainwindow.h"


MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow()
{
    instance = this;
    m_currentFrameIdx = 0;
    m_playing = false;

    resize(600,350);
    m_vboxLayout.setContentsMargins(5,5,5,5);
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(&m_vboxLayout);

    m_vboxLayout.addStretch();
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    m_vboxLayout.addLayout(hboxLayout);
    hboxLayout->addStretch();
    hboxLayout->addWidget(&m_label);
    hboxLayout->addStretch();
    m_vboxLayout.addStretch();

    QHBoxLayout *hboxLayoutBot = new QHBoxLayout();
    m_playButton = new QPushButton("Play", centralWidget());
    connect(m_playButton, SIGNAL(released()), this, SLOT(playButtonPushed()));
    hboxLayoutBot->addWidget(m_playButton);
    m_playButton->setFixedSize(90, 30);

    m_currentFrameBox = new QLineEdit(centralWidget());
    m_currentFrameBox->setValidator(new QIntValidator(-99999, 99999, this));
    connect(m_currentFrameBox, SIGNAL(editingFinished()), this, SLOT(currentFrameBoxSet()));
    hboxLayoutBot->addWidget(m_currentFrameBox);
    m_currentFrameBox->setFixedSize(50, 30);

    m_timeline = new Timeline(centralWidget());
    hboxLayoutBot->addWidget(m_timeline);
    m_vboxLayout.addLayout(hboxLayoutBot);

//    QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
}


void MainWindow::appendFrame(Frame *frame) {
    m_frames.append(frame);
    showFrame(m_frames.size()-1);
}


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


void MainWindow::showNextFrame() {
    if(m_frames.size() < 2) {
        if(m_playing)
            QTimer::singleShot(static_cast<int>(1000.0f/24.0f), this, SLOT(showNextFrame()));
        return;
    }

    int nextIdx = m_currentFrameIdx + 1;
    if(nextIdx >= m_frames.size())
        nextIdx = 0;

    showFrame(nextIdx);

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
    if(m_frames.size()==0) {
        m_currentFrameBox->setText("");
        setFocus();
        return;
    }

    int firstFrameNum = getFirstFrame()->m_frameNum;
    int lastFrameNum = getLastFrame()->m_frameNum;
    int setFrame = m_currentFrameBox->text().toInt();

    if(setFrame < firstFrameNum)
        setFrame = firstFrameNum;
    else if(setFrame > lastFrameNum)
        setFrame = lastFrameNum;

    int frameIdx = setFrame - firstFrameNum;
    showFrame(frameIdx);

    setFocus();
}


Frame *MainWindow::getCurrentFrame() {
    if(m_frames.size()==0)
        return nullptr;

    return m_frames[m_currentFrameIdx];
}

Frame *MainWindow::getFirstFrame() {
    if(m_frames.size()==0)
        return nullptr;

    return m_frames[0];
}

Frame *MainWindow::getLastFrame() {
    if(m_frames.size()==0)
        return nullptr;

    return m_frames.last();
}

int MainWindow::getNumFrames() {
    return m_frames.size();
}
