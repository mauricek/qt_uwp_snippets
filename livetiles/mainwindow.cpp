#include "mainwindow.h"
#include "livetile.h"
#include "ui_mainwindow.h"

LiveTile *tile = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    tile = new LiveTile;
    tile->setText("Yipie");
    tile->setDescription("Some more info text");
    tile->update();
}
