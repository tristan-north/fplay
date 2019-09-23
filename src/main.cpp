#include <QApplication>
#include <QDir>
#include <QRegularExpression>
#include "server.h"
#include "mainwindow.h"
#include "common.h"


#include "half.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create .flipbook_lock file
    QString fileText = QStringLiteral("0 %1\n").arg(PORT);
    QStringList houdiniDirs = QDir::home().entryList(QDir::Dirs);
    QRegularExpression re("houdini\\d\\d.\\d");
    QString hostname = QString::fromLocal8Bit(qgetenv("HOSTNAME"));

    for(int i=0; i<houdiniDirs.size(); i++) {
        if(houdiniDirs[i].contains(re)) {
            QString filePath = QDir::home().filePath(houdiniDirs[i] + "/.flipbook_lock." + hostname);
            QFile file(filePath);
            file.open(QIODevice::WriteOnly);
            file.write(fileText.toUtf8());
            file.close();
        }
    }

    // Create app window
    MainWindow mainWin;
    Server mServer;
    mainWin.show();

    return app.exec();
}


