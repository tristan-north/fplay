#include <QApplication>
#include "server.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWin;

    Server mServer;

    mainWin.show();

    return app.exec();
}


/* TODO

 - When clicking and dragging on the timeline it crashes when going too far left or right out of the bounds of the widget
 - Timeline doesn't respond to mouseclicks only click and drag

 - Frame timing would be more accurate if the time taken to render the frames and the exact time between the last frame shown was taken in to account


*/
