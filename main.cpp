#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("TINY扩充语言的语法分析_邓智超_20192121026");
    w.show();
    return a.exec();
}
