#ifndef FORM_H
#define FORM_H
#include "loginmanager.h"

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private slots:
    void on_pushButton_clicked();
    void stateUpdated(int);
    void errorUpdated(int);

private:
    Ui::Form *ui;
    LoginManager login;
};

#endif // FORM_H
