#include "mainwidget.h"
#include "ui_mainwidget.h"
#include"QtWidgets"
#include"QString"
#include"Implementations.h"
#include "QTextDocument"
#include "QTextBlock"
#include "QMenuBar"



MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    QMenu *menu = new QMenu("菜单");
    QAction *action1 = new QAction("打开文件");
    QList<QAction*> list;
    list.append(action1);
    menu->addActions(list);
    QMenuBar *menuBar=new QMenuBar;
    menuBar->addMenu(menu);
    menuBar->addSeparator();    //分隔栏
    ui->verticalLayout->addWidget(menuBar);
    connect(action1, &QAction::triggered, this, &MainWidget::openFile);
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::on_pushButton_clicked()
{
    letter_map.clear();//清空map的内容
    punctuation_map.clear();
    ui -> textBrowser -> setText("");//清空输出区域
    memset(str, '\0', sizeof (char) * 1024);//清空char数组
    QTextDocument * doc = ui -> plainTextEdit -> document();//获取输入框文本
    int lineNumber = doc -> blockCount();//行数
    for(int i = 0; i < lineNumber; i++){
        index = 0;//当前位置清0
        QString string = doc -> findBlockByNumber(i).text();//获取第i行文本
        QByteArray ba = string.toLocal8Bit();//string转换成char[]
        memcpy(str, ba.data(),ba.size() + 1);
        str[ba.size()] = ' ';//在末尾加上空格，防止最后一个单词不被记录
        str[ba.size() + 1] = '\0';//加上结尾'\0'
        doProcess(str);//进行处理
    }
    for(it1 = letter_map.begin(); it1 != letter_map.end(); it1++){
        qDebug() << it1.key() << "  "<< it1.value() << endl;
        ui -> textBrowser -> insertPlainText(it1.key() + "\t");
        ui -> textBrowser -> insertPlainText(it1.value() + "\n");
    }
    for(it2 = punctuation_map.begin(); it2 != punctuation_map.end(); it2++){
        //qDebug() << it2.key() << "  "<< it2.value() << endl;
        char arr[2];
        arr[0] = it2.key();
        arr[1] = '\0';
        ui -> textBrowser -> insertPlainText( QString::fromLocal8Bit(arr) + '\t');//char[]转换成QString输出
        ui -> textBrowser -> insertPlainText(it2.value() + "\n");
    }
}

void MainWidget::openFile()
{
    //qDebug() << "hello";
    ui -> plainTextEdit ->clear();
    QString s = QFileDialog::getOpenFileName(this,"选择文件(仅支持txt和cpp)","/", "Files(*.txt *.cpp)");
    QFile f(s);//打开文件
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text))//打开指定文件
    {
        QMessageBox::about(NULL, "文件", "文件打开失败");
    }
    QTextStream txtInput(&f);
    QString lineStr;
    while (!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();  //读取数据
        ui -> plainTextEdit ->insertPlainText(lineStr + '\n');
    }

}
