#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_tabWidget_clicked(int index);

    void on_button_clicked();

private:
    Ui::MainWidget *ui;
};

#endif // MAINWIDGET_H
