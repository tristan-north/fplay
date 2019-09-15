#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include "timeline.h"
#include "frame.h"
#include "seqlist.h"
#include "sequence.h"


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
    SeqList *seqList;
    int m_currentFrameNum;
    bool m_playing;

signals:

public slots:
    void playButtonPushed();
    void showNextFrame();
    void currentFrameBoxSet();

protected:
    void keyPressEvent(QKeyEvent *);

private:
    QLabel m_label;
    static MainWindow *instance;
    QPushButton *m_playButton;
    QLineEdit *m_currentFrameBox;
    Timeline *m_timeline;
    Sequence *m_currentlyPlayingSeq;
    Sequence *m_currentlyFlippingSeq;
    QTimer showNextFrameTimer;
};

#endif // MAINWINDOW_H

