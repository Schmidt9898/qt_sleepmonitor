#include "succeswindow.h"
#include "ui_succeswindow.h"

SuccesWindow::SuccesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SuccesWindow)
{
    ui->setupUi(this);
}

SuccesWindow::~SuccesWindow()
{
    delete ui;
}

void SuccesWindow::on_okButton_clicked()
{
    this->close();
}

