#include "mainwidget.h"
#include "ui_mainwidget.h"
#include"iostream"
using namespace std;

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this -> setWindowTitle("XLEX author: DzcGood 20192121026");

    //ui -> NFA -> setAttribute("string","DFA");
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::on_tabWidget_clicked(int index)
{
    switch (index) {
    case 0:
        //NFA
        break;
    case 1:
        //DFA
        break;
    case 2:
        //MinDFA
        break;
    case 3:
        //ToCode
        break;
    }
}

/**
* 点击按钮，按顺序执行tabWidget的四个槽函数
* 即： NFA -> DFA -> MinDFA -> ToCode
*/
void MainWidget::on_button_clicked()
{
    for(int i = 0; i < 4; i++)
    {
        on_tabWidget_clicked(i);
    }
}
