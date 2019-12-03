#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    MainWindow w;
    w.setWindowTitle(QObject::tr("MindViewer"));
    w.setWindowIcon(QIcon(":/resources/icons/mind.svg"));
    w.setMinimumSize(600,400);
    w.resize(800,600);
    w.show();
    return a.exec();
}
