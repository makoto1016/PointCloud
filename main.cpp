#include <QtGui/QApplication>
#include <QTextCodec>
//#include <GL/glew.h>
#include "MainWindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    //qWarning() <<     glewInit();
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
