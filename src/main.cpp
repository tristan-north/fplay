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


 - When launched create the necessary .flipbook_lock file
 - Add keyboard shortcuts
 - Frame timing would be more accurate if the time taken to render the frames and the exact time between the last frame shown was taken in to account
 - Support float image data

*/
