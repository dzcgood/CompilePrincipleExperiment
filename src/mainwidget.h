#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include"QPushButton"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    QMenu * menu;
    void openFile();
    ~MainWidget();
private slots:
    void on_pushButton_clicked();
private:
    Ui::MainWidget *ui;
};

#endif // MAINWIDGET_H
