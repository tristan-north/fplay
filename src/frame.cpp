#include <QtDebug>
#include <QtEndian>
#include "frame.h"
#include "half.h"
#include "mainwindow.h"

char *readElement(const uchar *data, uint &offset)
{
    uint elementSize;
    memcpy(&elementSize, data+offset, sizeof(uint));
    elementSize = qFromBigEndian(elementSize);
    if(elementSize == 0)
        return nullptr;

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

uint8_t get8BitValueFromHalfFloatChars(const uchar *inChars) {
    uchar chars[2];
    chars[0] = inChars[0];
    chars[1] = inChars[1];

    half_float::half n = *reinterpret_cast<half_float::half *>(chars);

    return uint8_t(n*255 + 0.5f);
}


Frame::Frame(const uchar *data) {
    m_resX = 0;
    m_resY = 0;
    bool isHDR = false;

    // --- Read header ---

    // Read the header strings
    uint offset = 0;
    while(true) {
        char *string = readElement(data, offset);
        if(string == nullptr)
            return;

        QString qString(string);

        // First header string has info about the image eg:
        // R 38584 1920 1080 1920 1080 1 C 0 4 0 255
        if(m_resX == 0) {
            QStringList list = qString.split(' ');
            m_resX = list[2].toInt();
            m_resY = list[3].toInt();

            if(list[11] != "255")
                isHDR = true;
        }

        // Get frame number from header
        if(qString.startsWith("currentframe")) {
            QStringList list = qString.split(' ');
            m_frameNum = list[1].toInt();
//            qInfo() << "currentframe from header: " << m_frameNum;
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
//    qInfo() << "imageDataSize: " << imageDataSize;
    offset += sizeof(uint) + 1;
    const uchar *pixelData = data+offset;

    // -------------------


    if(isHDR) {
        QImage img(m_resX, m_resY, QImage::Format_RGBX8888);

        for(int y=0; y<m_resY; y++) {
            uchar *scanline = img.scanLine(y);
            for(int x=0; x<m_resX; x++) {
                scanline[x*4] = get8BitValueFromHalfFloatChars(pixelData + y*m_resX*8 + x*8 + 0);
                scanline[x*4+1] = get8BitValueFromHalfFloatChars(pixelData + y*m_resX*8 + x*8 + 2);
                scanline[x*4+2] = get8BitValueFromHalfFloatChars(pixelData + y*m_resX*8 + x*8 + 4);
            }
        }

        img = img.mirrored(false, true);
        m_pixmap = QPixmap::fromImage(img);

    }
    else {
        QImage img(pixelData, m_resX, m_resY, QImage::Format_RGBX8888);
        img = img.mirrored(false, true);
        m_pixmap = QPixmap::fromImage(img);
    }



    // Figure out if this frame is a new sequence or not
    bool isNewSeq = false;
    MainWindow *mainWindow = MainWindow::getInstance();

    // It's the first sequence if currentlyFlippingSeq is null
    if(mainWindow->getFlippingSequence() == nullptr)
        isNewSeq = true;
    else {
        // If the new frame number is 1 after the last frame of currentlyFlippingSeq it's part of the same sequence
        int lastFrameNum = mainWindow->getFlippingSequence()->getLastFrame()->m_frameNum;
        if(m_frameNum != lastFrameNum+1) {
            isNewSeq = true;
        }
    }

    if(isNewSeq) {
        // If new resolution is bigger than current viewport size resize main win

        if(m_resX > mainWindow->getViewportSize().width() ||
                m_resY > mainWindow->getViewportSize().height())
            QTimer::singleShot(0, mainWindow, SLOT(resizeMainWindow()));


        Sequence *seq = mainWindow->seqList->newSequence();
        seq->appendFrame(this);
        mainWindow->setFlippingSequence(seq);
        mainWindow->setPlayingSequence(seq);

    } else {
        mainWindow->getFlippingSequence()->appendFrame(this);

        // If playback is stopped and the currently playing seq is the flipping seq show the frame
        if(!mainWindow->m_playing && mainWindow->getPlayingSequence() == mainWindow->getFlippingSequence())
            mainWindow->showFrame(this);
    }
}







