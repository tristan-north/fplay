#include <QPainter>

#include "playbutton.h"
#include "mainwindow.h"

PlayButton::PlayButton(QWidget *parent) : QAbstractButton(parent)
{
    m_playNormal = QPixmap(":/play.png");
    m_playDown = QPixmap(":/playDown.png");
}

void PlayButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    MainWindow *mainWin{MainWindow::getInstance()};

    QPainter p( this );

    if(mainWin->m_playing)
        p.drawPixmap(0, 0, m_playDown);
    else
        p.drawPixmap(0, 0, m_playNormal);
}
