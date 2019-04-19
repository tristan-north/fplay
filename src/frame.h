#ifndef FRAME_H
#define FRAME_H

#include <QPixmap>


class Frame
{
public:
    Frame(const uchar *data);
    QPixmap m_pixmap;

    int m_resX, m_resY;
    int m_frameNum;

private:

};

#endif // FRAME_H
