#include "failwindow.h"
#include "ui_failwindow.h"

FailWindow::FailWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FailWindow)
{
    ui->setupUi(this);
}

FailWindow::~FailWindow()
{
    delete ui;
}

void FailWindow::on_okButton_clicked()
{
    this->close();
}

