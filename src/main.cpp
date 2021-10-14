#include"QtWidgets"
#include"mainwidget.h"
#include"QStackedLayout"
using namespace std;
MainWidget *mainWidget = NULL;



void init()
{
    mainWidget = new MainWidget();
    mainWidget->resize(1000,800);
    mainWidget->setWindowTitle("c++单词拼装器");
    mainWidget->show();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    init();
    return a.exec();
}
