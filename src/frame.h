#ifndef FRAME_H
#define FRAME_H

#include <QPixmap>


class Frame : public QObject
{
    Q_OBJECT

public:
    Frame(const uchar *data);

    QPixmap m_pixmap;

    int m_resX, m_resY;
    int m_frameNum;

public slots:
    void finishInitInMainThread();

private:
    QImage m_qImage;
};

#endif // FRAME_H
