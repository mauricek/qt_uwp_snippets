#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(&login, &LoginManager::errorChanged, this, &Form::errorUpdated);
    connect(&login, &LoginManager::stateChanged, this, &Form::stateUpdated);

    stateUpdated(login.state());
    errorUpdated(login.error());

    bool valid = login.isValid();
    if (!valid) {
        ui->pushButton->setText(QLatin1String("No authentication available"));
        ui->pushButton->setEnabled(false);
    }
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    login.login();
}

void Form::stateUpdated(int s)
{
    QString stateText;
    switch (s) {
    case LoginManager::Idle:
        stateText = QLatin1String("Idle");
        break;
    case LoginManager::Verifying:
        stateText = QLatin1String("Verifying");
        break;
    case LoginManager::Done:
        stateText = QLatin1String("Done");
        break;
    default:
        stateText = QLatin1String("Unknown");
        break;
    }
    ui->statusLabel->setText(stateText);
}

void Form::errorUpdated(int e)
{
    QString errorText;
    switch (e) {
    case LoginManager::NoError:
        errorText = QLatin1String("No Error");
        break;
    case LoginManager::Error:
        errorText = QLatin1String("Error");
        break;
    default:
        errorText = QLatin1String("Unknown");
        break;
    }
    ui->errorLabel->setText(errorText);
}
