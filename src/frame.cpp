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

QString getBufferAsHexStr(const unsigned char* buf, int buffsize) {
    QString result;
    for(int i = 0; i < buffsize; ++i)
        result += QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
    return result;
}


Frame::Frame(const uchar *data) {
    m_resX = 0;
    m_resY = 0;

    // --- Read header ---

    // Read the header strings
    uint offset = 0;
    while(true) {
        char *string = readElement(data, offset);
        QString qString(string);

        // First header string has info about the image eg:
        // R 38584 1920 1080 1920 1080 1 C 0 4 0 255
        if(m_resX == 0) {
            QStringList list = qString.split(' ');
            m_resX = list[2].toInt();
            m_resY = list[3].toInt();
        }

        // Get frame number from header
        if(qString.startsWith("currentframe")) {
            QStringList list = qString.split(' ');
            m_frameNum = list[1].toInt();
            qInfo() << "currentframe from header: " << m_frameNum;
        }

//        qInfo() << "Data in: " << string;
//        qInfo() << "Data in hex: " << getBufferAsHexStr(reinterpret_cast<uchar*>(string), QString(string).length()+1);

        delete[] string;

        // Doesn't seem to be an easy way to tell when the headers are finished and the data starts but
        // the second last header seems to be P.
        if(qString == QString("P"))
            break;
    }

    offset += 25; // This gets offset to where the int containing pixel data size is.


    // Get number of bytes of pixel data int from the header
    uint imageDataSize;
    memcpy(&imageDataSize, data+offset, sizeof(uint));
    imageDataSize = qFromBigEndian(imageDataSize);
    qInfo() << "imageDataSize: " << imageDataSize;
    offset += sizeof(uint) + 1;

    // -------------------


    QImage img(data+offset, m_resX, m_resY, QImage::Format_RGBX8888);
    img = img.mirrored(false, true);
    m_pixmap = QPixmap::fromImage(img);


    // Figure out if this frame is a new sequence or not
    MainWindow *mainWindow = MainWindow::getInstance();

    // It's the first sequence if currentlyFlippingSeq is null
    if(mainWindow->getFlippingSequence() == nullptr) {
//        mainWindow->currentlyFlippingSeq = mainWindow->seqList->newSequence();
//        mainWindow->currentlyPlayingSeq = mainWindow->currentlyFlippingSeq;

        Sequence *seq = mainWindow->seqList->newSequence();
        mainWindow->setFlippingSequence(seq);
        mainWindow->setPlayingSequence(seq);
    } else {
        // If the new frame number is 1 after the last frame of currentlyFlippingSeq it's part of the same sequence
        int lastFrameNum = mainWindow->getFlippingSequence()->getLastFrame()->m_frameNum;
        if(m_frameNum != lastFrameNum+1) {
            Sequence *seq = mainWindow->seqList->newSequence();
            mainWindow->setFlippingSequence(seq);
            mainWindow->setPlayingSequence(seq);
        }

    }

    mainWindow->getFlippingSequence()->appendFrame(this);
}







