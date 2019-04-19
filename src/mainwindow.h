#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include "timeline.h"
#include "frame.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow();
    static MainWindow *getInstance() { return instance; }
    void appendFrame(Frame *frame);
    void showFrame(int frameIdx);
    int m_currentFrameIdx;
    Frame *getCurrentFrame();
    Frame *getFirstFrame();
    Frame *getLastFrame();
    int getNumFrames();

signals:

public slots:
    void showNextFrame();
    void playButtonPushed();
    void currentFrameBoxSet();

private:
    QLabel m_label;
    static MainWindow *instance;
    QVector<Frame*> m_frames;
    QVBoxLayout m_vboxLayout;
    QPushButton *m_playButton;
    QLineEdit *m_currentFrameBox;
    Timeline *m_timeline;
    bool m_playing;
};

#endif // MAINWINDOW_H

