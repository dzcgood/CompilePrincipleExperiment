#include "mainwidget.h"
#include "ui_mainwidget.h"
#include"iostream"
#include"Utility.h"
using namespace std;

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui -> input -> setText("(a|b)*cd");
    this -> setWindowTitle("XLEX author: DzcGood 20192121026");

    //ui -> NFA -> setAttribute("string","DFA");
}

MainWidget::~MainWidget()
{
    delete ui;
}



/**
* 即： NFA -> DFA -> MinDFA -> ToCode
*/
void MainWidget::on_button_clicked()
{
    //获取正则表达式
    string regExp = ui -> input ->text().toStdString();
    //构建代理类，转换过程在构造函数中一并完成
    Proxy proxy(regExp);
    //先显示NFA
    QTableWidget * nfaTable = ui -> nfaTable;
    int length = proxy.nfa.nodeNumber;
    cout<<length;
    //设置表格长宽
    nfaTable -> setRowCount(length);
    nfaTable -> setColumnCount(length);
    //显示
    for(int i = 0; i < length; i++)
    {
        for(int j = 0; j < length; j++)
        {
            string ch = "";
            ch += proxy.chart[i+1][j+1];
            if(ch == " ")   ch = "None";
            QString item = QString::fromStdString(ch);
            nfaTable -> setItem(i, j, new QTableWidgetItem(item));
        }
    }

    //接下来显示DFA
    QTableWidget *dfaTable = ui-> dfaTable;
    int colCount = proxy.dfa.wordList.size();
    //有几个EPSILION集合，就有几行
    int rowCount = proxy.dfa.graph.size();
    dfaTable->setRowCount(rowCount);
    dfaTable->setColumnCount(colCount);

    //表格的行标题和列标题
    QStringList colNames;
    QStringList rowNames;
    //设置行名，其实就是把每个集合转成字符加进去
    for(int j = 0; j < rowCount; j++)
    {
        string node = proxy.dfa.graph[j]->toString();
        rowNames.append(QString::fromStdString(node));
    }

    DFA dfa = proxy.dfa;
    //设置列名
    int i = 0;
    //遍历设置行名
    for(char ch : proxy.dfa.wordList)
    {
        string col = "";
        col += ch;
        colNames.push_back(QString::fromStdString(col));
        //设置第j行第i列的值
        for(int j = 0; j < rowCount; j++)
        {
            string transformResult;
            //找到第j个DFANode在处理字符ch后转向哪个DFANode
            DFANode * node = proxy.dfa.graph[j] -> process(ch);
            if(node == NULL)
            {
                transformResult = "(None)";
            }
            else
            {
                transformResult = node -> toString();
            }
            dfaTable->setItem(j,i,new QTableWidgetItem(QString::fromStdString(transformResult)));
        }
        i++;
    }

    dfaTable->setHorizontalHeaderLabels(colNames);
    dfaTable->setVerticalHeaderLabels(rowNames);

    //
    string startNodeText = "";
    string endNodeText = "";
    for(DFANode *n : proxy.dfa.graph)
    {
        if(n->contains(1))
            startNodeText = n->toString();
        if(n->state == END)
        {
            endNodeText += " " + n->toString();
        }
    }

    //最小化DFA
    proxy.minimizeDFA();
    QTableWidget *minDfaTable = ui -> minDfaTable;
    rowCount = proxy.finalDFA.graph.size();
    minDfaTable->setRowCount(rowCount);
    minDfaTable->setColumnCount(colCount);

    rowNames.clear();
    for(int j = 0; j < rowCount; j++)
    {
        string node = proxy.finalDFA.graph[j]->minName;
        rowNames.append(QString::fromStdString(node));
    }
    i = 0;
    for(char ch : proxy.dfa.wordList)
    {
        for(int j = 0; j < rowCount; j++)
        {
            string transformResult;
            DFANode *node = proxy.finalDFA.graph[j]->process(ch);
            if(node == NULL)
            {
                transformResult = "(None)";
            }
            else
            {
                transformResult = node->minName;
            }
            minDfaTable->setItem(j, i, new QTableWidgetItem(QString::fromStdString(transformResult)));
        }
        i++;
    }
    minDfaTable->setHorizontalHeaderLabels(colNames);
    minDfaTable->setVerticalHeaderLabels(rowNames);

    startNodeText = "";
    endNodeText = "";
    for(DFANode *n : proxy.finalDFA.minEndNodes)
    {
        endNodeText += " " + n->minName;
    }
    startNodeText = proxy.finalDFA.minStartNode->minName;





    proxy.generateCode();
    QTextEdit * codeText = ui-> codeBrowser;
    codeText->setText(QString::fromStdString(proxy.code));

}
