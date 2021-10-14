#ifndef MAIN_H
#define MAIN_H

#include <QWidget>

namespace Ui {
class Main;
}

class Main : public QWidget
{
    Q_OBJECT

public:
    explicit Main(QWidget *parent = nullptr);
    ~Main();

private:
    Ui::Main *ui;
};

#endif // MAIN_H
