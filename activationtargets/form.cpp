#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::setActivationFilter(ActivationFilter *f)
{
    filter = f;

    connect(filter, &ActivationFilter::protocolActivation, this, &Form::protocolActivated);
    connect(filter, &ActivationFilter::shareActivationStarted, this, &Form::shareActivated);
    connect(filter, &ActivationFilter::fileActivation,this, &Form::fileActivated);
}

void Form::on_successButton_clicked()
{
    // report success to the filter
    filter->finishShareActivation();
}

void Form::on_errorButton_clicked()
{
    // report error to the filter
    filter->finishShareActivation(true, QLatin1String("We requested an error report"));
}

void Form::protocolActivated(const QString &p)
{
    ui->activationLabel->setText(QLatin1String("Protocol Activation"));
    ui->protocolLabel->setText(p);
}

void Form::shareActivated()
{
    ui->activationLabel->setText(QLatin1String("Share Target Activation"));
    ui->successButton->setEnabled(true);
    ui->errorButton->setEnabled(true);
}

void Form::fileActivated(const QStringList &files)
{
    ui->activationLabel->setText(QLatin1String("File Activation"));
    ui->fileList->clear();
    ui->fileList->insertItems(0, files);
}
