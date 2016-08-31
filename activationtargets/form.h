#ifndef FORM_H
#define FORM_H
#include "activationfilter.h"
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

    void setActivationFilter(ActivationFilter *f);
private slots:
    void on_successButton_clicked();
    void on_errorButton_clicked();

    void protocolActivated(const QString &p);
    void shareActivated();
    void fileActivated(const QStringList &files);

private:
    Ui::Form *ui;
    ActivationFilter *filter;
};

#endif // FORM_H
