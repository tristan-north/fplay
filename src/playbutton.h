#ifndef PLAYBUTTON_H
#define PLAYBUTTON_H

#include <QWidget>
#include <QAbstractButton>


class PlayButton : public QAbstractButton
{
public:
    PlayButton(QWidget *parent = nullptr);
    QSize sizeHint() const { return m_playNormal.size(); }

protected:
    void paintEvent(QPaintEvent* e);

private:
    QPixmap m_playNormal;
    QPixmap m_playDown;
};

#endif // PLAYBUTTON_H
