#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"QFileDialog"
#include"QTextStream"
#include"Processor.h"
#include"string"
#include"QMessageBox"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
    点击打开文件按钮后，执行的动作
    把文件内容读到testEdit中
*/
void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个文法规则文件", ".", "Code(*.txt *.tiny )");
    ui->lineEdit->setText(file_path);
    ui->textEdit->clear();
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
       return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        ui->textEdit->append(line);
    }
    file.close();
}

/**
    “分析”按钮执行的动作
*/
void MainWindow::on_pushButton_2_clicked()
{
    std::string analyzerFilePath = ui->lineEdit->text().toStdString();
    //未输入文法
    if(analyzerFilePath.empty())
    {
        QMessageBox::about(NULL,"Warning",
                           "Please select a file as an input");
        return;
    }
    processor = new Processor(analyzerFilePath);
    processor->log << "-------------原本语法如下---------------" << '\n';
    processor->printGrammers();
    processor->dealLeftRecursion();
    processor->log << "-------------去除左递归后---------------" << '\n';
    processor->printGrammers();
    processor->simplify();
    processor->log << "-------------去除多余项后---------------" << '\n';
    processor->printGrammers();
    processor->dealLeftCommonFacotr();
    processor->log << "-------------去除左公因子后---------------" << '\n';
    processor->printGrammers();
    processor->getFirst();
    processor->getFollow();
    //关闭输入流
    processor->log.close();
    //接下来输出
    //先输出处理后的文法
    string output;
    output = this->getProcessedGrammers();
    this -> ui-> textBrowser_6->setText(QString::fromStdString(output));
    //输出first集合
    this->outputFirst();
    //输出follow集合
    this->outputFollow();
    //输出日志信息
    this->outputLog();
}


/**
    输出给定句子的最左推导过程
*/
void MainWindow::on_pushButton_4_clicked()
{
    //未输入文法或未分析
    if(processor == NULL)
    {
        QMessageBox::about(NULL,"Warning",
                           "Please click the \"分析\" button before derivation");
        return;
    }
    //清空上次输入的句子
    processor->input.clear();
    //读取新的句子
    string str = ui -> lineEdit_2 ->text().toStdString();
    //判空
    if(str.empty())
    {
        QMessageBox::about(NULL,"Warning", "输入句子为空");
        return;
    }
    //将句子转成vector，存入processor->input
    for(size_t i = 0; i<str.length();i++)
    {
        string s = "";
        processor->input.push_back(s+str[i]);
    }
    //获取输出
    vector<vector<string>> out = processor->leftMostDerivation();
    string output = "";
    output +="-------------处理后的文法为---------------\n";
    output += getProcessedGrammers();
    output +="-------------最左推导过程为---------------\n";
    for(vector<string> v : out)
    {
        output += "=>";
        for(string s : v)
        {
            output += s;
        }
        output += "\n";
    }
    this -> ui -> textBrowser_3-> setText(QString::fromStdString(output));
}


string MainWindow::getProcessedGrammers()
{
    string output = "";
    //输出
    for(LinkNode node : this -> processor->grammers)
    {
        output += node.left;
        output += "->";
        for(size_t j = 0; j < node.right.size(); j++)
        {
            for(size_t i = 0; i < node.right[j].size(); i++)
                output += node.right[j][i];
            if(j != node.right.size() - 1)
                output += "|";
        }
        output += "\n";
    }
    return output;
}

void MainWindow::outputFirst()
{
    string s = "";
    for(LinkNode node : this -> processor->grammers)
    {
        s += "first(" + node.left + ") = {";
        set<string> firstSet = this -> processor ->first[node.left];
        set<string>::iterator it = firstSet.begin();
        bool flag = false;
        for(; it != firstSet.end(); it++)
        {
            if(flag)
                s += ",";
            s += *it;
            flag = true;
        }
        s += "}\n";
    }
    this -> ui -> textBrowser_2->setText(QString::fromStdString(s));
}

void MainWindow::outputFollow()
{
    string s = "";
    //输出follow集合到log文件
    map<string, set<string>>::iterator it = processor->follow.begin();
    for(; it != processor->follow.end(); it++)
    {
        s += "follow(" + it->first + ") = {";
        set<string> &temp = it->second;
        bool flag = false;
        for(set<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
        {
            if(flag) s += ",";
            s += *it1;
            flag = true;
        }
        s += "}\n";
    }
    this -> ui -> textBrowser_4->setText(QString::fromStdString(s));
}

void MainWindow::outputLog()
{
    ifstream log;
    log.open("log.txt");
    if(!log.is_open())
    {
        QMessageBox::about(NULL,"Warning","Fail to open \"log.txt\"");
        return;
    }
    string s = "";
    string line;
    while(getline(log,line))
        s += line + "\n";
    log.close();
    this -> ui -> textBrowser->setText(QString::fromStdString(s));
}


