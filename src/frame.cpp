#include <QtDebug>
#include <QtEndian>
#include "frame.h"
#include "mainwindow.h"

char *readElement(const uchar *data, uint &offset)
{
    uint elementSize;
    memcpy(&elementSize, data+offset, sizeof(uint));
    elementSize = qFromBigEndian(elementSize);

    offset += sizeof(uint) + 1;

    char *dataString = new char[elementSize];
    memcpy(dataString, data + offset, elementSize);

    // Replace the null seperators with spaces to be able to print it out
    for(uint i=0; i<elementSize-1; i++) {
        if(dataString[i] == '\0') dataString[i] = ' ';
    }

    offset += elementSize;

    return dataString;
}


Frame::Frame(const uchar *data) {
    // --- Read header ---

    // Read the 22 header strings
    uint offset = 0;
    for(int i = 0; i < 22; i++) {
        char *string = readElement(data, offset);

        // First header string has info about the image eg:
        // R 38584 1920 1080 1920 1080 1 C 0 4 0 255
        if(i==0) {
            QStringList list = QString(string).split(' ');
            m_resX = list[2].toInt();
            m_resY = list[3].toInt();
        }

        // 20th header string has frame number
        if(i==19) {
            QStringList list = QString(string).split(' ');
            m_frameNum = list[1].toInt();
        }

//        qInfo() << "Data in: " << string;
        delete[] string;
    }


    // Get number of bytes of pixel data int from the header
    uint imageDataSize;
    memcpy(&imageDataSize, data+offset, sizeof(uint));
    imageDataSize = qFromBigEndian(imageDataSize);
//    qInfo() << "imageDataSize: " << imageDataSize;
    offset += sizeof(uint) + 1;

    // -------------------


    QImage img(data+offset, m_resX, m_resY, QImage::Format_RGBX8888);
    img = img.mirrored(false, true);
    m_pixmap = QPixmap::fromImage(img);

    MainWindow::getInstance()->appendFrame(this);
}


