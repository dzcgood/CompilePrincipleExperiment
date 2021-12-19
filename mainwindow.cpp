#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include "GLOBALS.h"
#include "PARSE.h"
#include "SCAN.h"
#include "UTIL.h"
#include <string>
using namespace std;

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;
FILE * log1;
FILE * log2;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE; //只追踪parse
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int Error = FALSE;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //输出日志的路径
    l1_path = QDir::currentPath() + "/log1.txt";
    l2_path = QDir::currentPath() + "/log2.txt";
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * open按钮
 * 读取代码文件到ui->textEdit中
 */
void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Code(*.txt *.tiny )");
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

/*
 * analyse按钮
 * 生成语法树并输出
 */
void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->clear();
    QString file_path = ui->lineEdit->text();

    string temp1 = file_path.toStdString();
    string temp2 = l1_path.toStdString();
    string temp3 = l2_path.toStdString();

    const char *pgm = (char*)temp1.c_str();
    const char *log1_path = (char *)temp2.c_str();
    const char *log2_path = (char *)temp3.c_str();

    source = fopen(pgm,"rw");
    if(source == NULL)
    {
        fprintf(stderr,"File %s not found\n",pgm);
        exit(1);
    }


    log1 = fopen(log1_path, "wb");
    log2 = fopen(log2_path, "wb");

    fprintf(log1, "Open file %s successfully", pgm);

    listing = log2; /* send listing to screen */

    TreeNode *syntaxTree;
    syntaxTree = parse();

    //追踪parse过程，将SyntaxTree打印到log2中
    if(TraceParse){
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }

    fclose(source);
    fclose(log1);
    fclose(log2);

    //从log2读出SyntaxTree到textEdit_2
    file_path = this->l2_path;
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
       return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        ui->textEdit_2->append(line);
    }

    //重置Scan相关的参数，以便下次使用
    resetScan();
}



/*
 * save按钮
 * 保存textEdit中编辑的代码到指定路径
 */
void MainWindow::on_pushButton_3_clicked()
{
    QString save_path = ui->lineEdit->text();
    if(save_path.isEmpty())
    {
        QMessageBox::information(this,"Error","Please open a source first!");
        return;
    }
    QFile *file = new QFile;
    file->setFileName(save_path);
    if(file->open(QIODevice::WriteOnly))
    {
        QTextStream out(file);
        out << ui->textEdit->toPlainText();
        file->close();
        delete file;
        QMessageBox::information(this,"Done", "Sucessfully save file!");
    }
    else
    {
        QMessageBox::information(this,"Error", "Fail to save file!");
        delete file;
        return;
    }
}
