#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QWidget>
#include "frame.h"

class Sequence : public QWidget
{
    Q_OBJECT
public:
    explicit Sequence(QWidget *parent = nullptr);
    void appendFrame(Frame *frame);
    int getNumFrames();
    Frame *getFrameByFrameNum(int frameNum);
    Frame *getFrameByIndex(int frameIdx);
    Frame *getLastFrame();
    void deleteSeq();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event);

private:
    QVector<Frame*> m_frames;
    QString m_title;

signals:

public slots:
};

#endif // SEQUENCE_H
