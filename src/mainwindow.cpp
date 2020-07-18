#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QValidator>
#include <QKeyEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyle>
#include <QFileDialog>
#include <QProcess>
#include "mainwindow.h"
#include "common.h"


MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow() : m_currentFrameNum(-1), m_playing(false), m_currentlyPlayingSeq(nullptr), m_currentlyFlippingSeq(nullptr), m_exportFilePath("./fplay_export.mov")
{
    instance = this;
    showNextFrameTimer.setInterval(int(1000.0f/24.0f));
    connect(&showNextFrameTimer, SIGNAL(timeout()), this, SLOT(showNextFrame()));

    resize(900,550);
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Window, QColor(46,46,46));
    setAutoFillBackground(true);
    setPalette(pal);

    QHBoxLayout *rootHboxLayout = new QHBoxLayout();
    rootHboxLayout->setContentsMargins(0,0,0,0);
    rootHboxLayout->setSpacing(0);
    setCentralWidget(new QWidget());
    centralWidget()->setLayout(rootHboxLayout);

    // Sequence list
    seqList = new SeqList(centralWidget());
    seqList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    seqScrollArea = new QScrollArea;
    seqScrollArea->setFocusPolicy(Qt::NoFocus);

    seqScrollArea->verticalScrollBar()->setStyleSheet(QString::fromUtf8(
           "QScrollBar:vertical {"
            "    border: 1px solid rgb(20, 20, 20);"
            "    background: rgb(32, 32, 32);"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"

            "QScrollBar::handle:vertical {"
            "    background: rgb(60, 60, 60);"
            "    min-height: 0px;"
            "    border-radius: 4px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "    subcontrol-position: bottom;"
            "    subcontrol-origin: margin;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0 px;"
            "    subcontrol-position: top;"
            "    subcontrol-origin: margin;"
            "}"
            ));


    seqScrollArea->setWidgetResizable(true);
    seqScrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    seqScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    seqScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    seqScrollArea->setWidget(seqList);
    rootHboxLayout->addWidget(seqScrollArea);

    // Image viewport
    QPalette viewportBkgPalette = QApplication::palette();
    viewportBkgPalette.setColor(QPalette::Window, QColor(0,0,0));

    m_viewportBkgTop = new QWidget();
    m_viewportBkgTop->setPalette(viewportBkgPalette);
    m_viewportBkgTop->setAutoFillBackground(true);
    m_viewportBkgTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_viewportBkgLeft = new QWidget();
    m_viewportBkgLeft->setPalette(viewportBkgPalette);
    m_viewportBkgLeft->setAutoFillBackground(true);
    m_viewportBkgLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_viewportBkgRight = new QWidget();
    m_viewportBkgRight->setPalette(viewportBkgPalette);
    m_viewportBkgRight->setAutoFillBackground(true);
    m_viewportBkgRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_viewportBkgBot = new QWidget();
    m_viewportBkgBot->setPalette(viewportBkgPalette);
    m_viewportBkgBot->setAutoFillBackground(true);
    m_viewportBkgBot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *vboxLayout = new QVBoxLayout();
    rootHboxLayout->addLayout(vboxLayout);
    vboxLayout->addWidget(m_viewportBkgTop);
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    vboxLayout->addLayout(hboxLayout);
    hboxLayout->addWidget(m_viewportBkgLeft);
    m_label.hide();

    hboxLayout->addWidget(&m_label);
    hboxLayout->addWidget(m_viewportBkgRight);
    vboxLayout->addWidget(m_viewportBkgBot);

    // Play button
    QHBoxLayout *hboxLayoutBot = new QHBoxLayout();
    hboxLayoutBot->setContentsMargins(0,0,5,5);
    hboxLayoutBot->setSpacing(5);
    hboxLayoutBot->addSpacing(5);
    m_playButton = new PlayButton(centralWidget());
    m_playButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

//    QPalette playPal = QApplication::palette();
//    playPal.setColor(QPalette::Button, QColor(50,50,50));
//    playPal.setColor(QPalette::ButtonText, QColor(TEXTCOLOR));
//    playPal.setColor(QPalette::Window, QColor(30,30,30)); // The button border
//    m_playButton->setPalette(playPal);

    connect(m_playButton, SIGNAL(released()), this, SLOT(playButtonPushed()));
    hboxLayoutBot->addWidget(m_playButton);
    m_playButton->setFocusPolicy(Qt::NoFocus);

    // Current frame text box
    m_currentFrameBox = new QLineEdit(centralWidget());
//    m_currentFrameBox->setFocusPolicy(Qt::NoFocus);
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

    // Export button
    m_exportButton = new QPushButton(centralWidget());
    m_exportButton->setFocusPolicy(Qt::NoFocus);
    m_exportButton->setIcon(style()->standardIcon(QStyle::SP_DriveFDIcon));
    m_exportButton->setStyleSheet("color: rgb(200, 200, 200); background-color: rgb(50, 50, 50);");
    m_exportButton->setFixedSize(30, 30);
    hboxLayoutBot->addWidget(m_exportButton);
    connect(m_exportButton, SIGNAL(released()), this, SLOT(exportButtonPushed()));

    vboxLayout->addSpacing(4);
    vboxLayout->addLayout(hboxLayoutBot);


    setFocus(); // Set focus to main window otherwise the frame num text widget gets it on startup

}


void MainWindow::showFrame(Frame *frame) {
    if(frame == nullptr) {
        static QPixmap nullPixmap;
        m_label.setPixmap(nullPixmap);

        m_currentFrameNum = 0;

        m_timeline->update();
        m_currentFrameBox->setText("");
        return;
    }

    m_label.setPixmap(frame->m_pixmap);
    m_label.resize(frame->m_resX, frame->m_resY);

    m_currentFrameNum = frame->m_frameNum;

    m_timeline->update();
    m_currentFrameBox->setText(QString::number(frame->m_frameNum));
}


void MainWindow::showNextFrame() {
    if(!m_currentlyPlayingSeq)
        return;

    if(m_currentlyPlayingSeq->getNumFrames() < 2) {
       return;
    }

    // If current frame is the last frame of the sequence, show the first frame next
    Frame *frame;
    if(m_currentFrameNum == m_currentlyPlayingSeq->getLastFrame()->m_frameNum)
        frame = m_currentlyPlayingSeq->getFrameByIndex(0);
    else
        frame = m_currentlyPlayingSeq->getFrameByFrameNum(m_currentFrameNum+1);

    showFrame(frame);
}


void MainWindow::playButtonPushed() {
    if(!m_currentlyPlayingSeq)
        return;

    if(m_playing) {
        m_playButton->setText("Play");
        m_playing = false;
        showNextFrameTimer.stop();
    }
    else {
        m_playButton->setText("Stop");
        m_playing = true;
        showNextFrame();
        showNextFrameTimer.start();
    }
}

void MainWindow::exportButtonPushed()
{
    if(!m_currentlyPlayingSeq)
        return;


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), m_exportFilePath, tr("Movies (*.mov)"));
    if(fileName=="")
        return;
    m_exportFilePath = fileName;

    QDir tmpDir("/var/tmp/fplay_export");
    tmpDir.mkpath(".");
    foreach(QString dirFile, tmpDir.entryList(QStringList("*"))) {
        tmpDir.remove(dirFile);
    }


    for(int i=0; i < m_currentlyPlayingSeq->getNumFrames(); i++) {
        Frame *frame = m_currentlyPlayingSeq->getFrameByIndex(i);

        frame->m_pixmap.save(tmpDir.path() + QString("/fplay_export.%1.jpg").arg(QString::number(frame->m_frameNum), 4, '0'), Q_NULLPTR, 100);
    }


    int firstFrameNum = m_currentlyPlayingSeq->getFrameByIndex(0)->m_frameNum;
    int timecodeFrames = firstFrameNum % 24;
    int timecodeSeconds = firstFrameNum / 24;
    int timecodeMinutes = timecodeSeconds / 60;
    timecodeSeconds = timecodeSeconds - timecodeMinutes*60;
    QString timecode = QString("timecode=00:%1:%2:%3").arg(QString::number(timecodeMinutes), 2, '0').arg(QString::number(timecodeSeconds), 2, '0').arg(QString::number(timecodeFrames), 2, '0');


    QProcess ffmpeg;
    ffmpeg.start("ffmpeg", QStringList() <<
                 "-y" <<           // Force overrwite
                 "-r" << "24" <<   // Frame rate
                 "-pattern_type" << "glob" <<
                 "-i" << QString("%1/fplay_export.*.jpg").arg(tmpDir.path()) <<
                 "-vcodec" << "mjpeg" <<
                 "-q:v" << "1" <<  // Quality
                 "-metadata" << timecode <<
                 "-aspect" << QString("%1:%2").arg(m_currentlyPlayingSeq->getFrameByIndex(0)->m_resX).arg(m_currentlyPlayingSeq->getFrameByIndex(0)->m_resY) <<
//                 "-metadata" << "\"PixelAspectRatio=1\"" <<

                 fileName);
    ffmpeg.waitForFinished();

//    qWarning() << ffmpeg.readAllStandardError();
//    qWarning() << ffmpeg.readAllStandardOutput();

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
        if(seqList->numSequences() == 0)
            return;

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
            m_label.hide();
        }
        else if(currentIndex == 0)
            setPlayingSequence(seqList->getSequenceByIndex(currentIndex+1));
        else
            setPlayingSequence(seqList->getSequenceByIndex(currentIndex-1));

        seqToDelete->deleteSeq();
        delete seqToDelete;
    }
    else if (event->key() == Qt::Key_Space) {
        playButtonPushed();
    }
    else if (event->key() == Qt::Key_Up) {
        int currSeqIndex = seqList->getSequenceIndex(getPlayingSequence());

        if(currSeqIndex > 0)
            setPlayingSequence(seqList->getSequenceByIndex(currSeqIndex-1));
    }
    else if (event->key() == Qt::Key_Down) {
        int currSeqIndex = seqList->getSequenceIndex(getPlayingSequence());

        if(currSeqIndex < seqList->numSequences()-1)
            setPlayingSequence(seqList->getSequenceByIndex(currSeqIndex+1));
    }
    else if (event->key() == Qt::Key_Right) {
        if(!m_currentlyPlayingSeq)
            return;

        if(m_playing)
            playButtonPushed();

        Frame *frame;
        int frameNumToShow = m_currentFrameNum + 1;
        if(frameNumToShow > m_currentlyPlayingSeq->getLastFrame()->m_frameNum)
            frame = m_currentlyPlayingSeq->getFrameByIndex(0);
        else
            frame = m_currentlyPlayingSeq->getFrameByFrameNum(frameNumToShow);

        showFrame(frame);
    }
    else if (event->key() == Qt::Key_Left) {
        if(!m_currentlyPlayingSeq)
            return;

        if(m_playing)
            playButtonPushed();

        Frame *frame;
        int frameNumToShow = m_currentFrameNum - 1;
        if(frameNumToShow < m_currentlyPlayingSeq->getFrameByIndex(0)->m_frameNum)
            frame = m_currentlyPlayingSeq->getLastFrame();
        else
            frame = m_currentlyPlayingSeq->getFrameByFrameNum(frameNumToShow);

        showFrame(frame);
    }
    else if (event->key() == Qt::Key_Home) {
        if(!m_currentlyPlayingSeq)
            return;

        if(m_playing)
            playButtonPushed();

        showFrame(m_currentlyPlayingSeq->getFrameByIndex(0));
    }
    else if (event->key() == Qt::Key_End) {
        if(!m_currentlyPlayingSeq)
            return;

        if(m_playing)
            playButtonPushed();

        showFrame(m_currentlyPlayingSeq->getLastFrame());
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
    m_label.show();  // Show incase image widget is currently hidden like if there are no prior sequences

    m_currentlyPlayingSeq = seq;
    seqScrollArea->ensureWidgetVisible(seq, 0, 0);

    Frame *frame = seq->getFrameByFrameNum(m_currentFrameNum);
    if(frame != nullptr)
        showFrame(frame);

    for(int i=0; i<seqList->numSequences(); i++) {
        seqList->getSequenceByIndex(i)->repaint();
    }

}

void MainWindow::setFlippingSequence(Sequence *seq)
{
    m_currentlyFlippingSeq = seq;
}

QSize MainWindow::getViewportSize()
{
    QSize size;

    if(m_label.isVisible()) {
        size.setWidth(m_label.width() + m_viewportBkgLeft->width() + m_viewportBkgRight->width());
        size.setHeight(m_label.height() + m_viewportBkgTop->height() + m_viewportBkgBot->height());
    }
    else {
        size.setWidth(m_viewportBkgLeft->width() + m_viewportBkgRight->width());
        size.setHeight(m_viewportBkgTop->height() + m_viewportBkgBot->height());
    }

    return size;
}

void MainWindow::resizeMainWindow()
{
    int margin = 30;

    int newWidth = m_label.size().width() + seqList->size().width() + margin*2;
    int newHeight = m_label.size().height() + m_playButton->size().height() + margin*2 + 9; // +9 to account for padding

    // Don't resize window to be smaller in either width or height
    if(newWidth < size().width())
        newWidth = size().width();
    if(newHeight < size().height())
        newHeight = size().height();

    resize(newWidth, newHeight);
}



