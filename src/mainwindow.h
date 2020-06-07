#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QScrollArea>
#include "timeline.h"
#include "frame.h"
#include "seqlist.h"
#include "sequence.h"
#include "playbutton.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow();
    static MainWindow *getInstance() { return instance; }
    void showFrame(Frame *frame);
    Sequence *getPlayingSequence();
    Sequence *getFlippingSequence();
    void setPlayingSequence(Sequence *seq);
    void setFlippingSequence(Sequence *seq);
    QSize getViewportSize();
    SeqList *seqList;
    QScrollArea *seqScrollArea;
    int m_currentFrameNum;
    bool m_playing;

signals:

public slots:
    void playButtonPushed();
    void exportButtonPushed();
    void showNextFrame();
    void currentFrameBoxSet();
    void resizeMainWindow();

protected:
    void keyPressEvent(QKeyEvent *);

private:
    QLabel m_label;
    static MainWindow *instance;
    PlayButton *m_playButton;
    QPushButton *m_exportButton;
    QLineEdit *m_currentFrameBox;
    Timeline *m_timeline;
    Sequence *m_currentlyPlayingSeq;
    Sequence *m_currentlyFlippingSeq;
    QTimer showNextFrameTimer;
    QWidget *m_viewportBkgTop, *m_viewportBkgBot, *m_viewportBkgLeft, *m_viewportBkgRight;
    QString m_exportFilePath;
};

#endif // MAINWINDOW_H

