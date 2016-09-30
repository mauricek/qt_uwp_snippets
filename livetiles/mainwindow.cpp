#include "mainwindow.h"
#include "livetile.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tile = new LiveTile;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    tile->setTitle(ui->titleEdit->text());
    tile->setText(ui->contentEdit->text());
    tile->update();
}
