#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"
#include "recorder.h"

SleepMonitorMain::SleepMonitorMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SleepMonitorMain)
{

    ui->setupUi(this);
}

SleepMonitorMain::~SleepMonitorMain()
{
    delete ui;
}


void SleepMonitorMain::on_startRecordingButton_clicked()
{

}



void SleepMonitorMain::on_recordTimeMin_valueChanged(int arg1)
{
    recordMinute = arg1;
    std::cout << recordMinute;
    if (recordMinute == 0 && recordHour == 0)   ui->startRecordingButton->setEnabled(false);
    else    ui->startRecordingButton->setEnabled(true);
}


void SleepMonitorMain::on_recordTimeHour_valueChanged(int arg1)
{
    recordHour = arg1;
    std::cout << recordHour;
    if (recordMinute == 0 && recordHour == 0)   ui->startRecordingButton->setEnabled(false);
    else    ui->startRecordingButton->setEnabled(true);
}


void SleepMonitorMain::on_connectButton_clicked()
{
    //cameraThread = std::thread(startCamera);
    ui->startRecordingButton->setEnabled(false);
    std::cout << GetCamera() << "\n";
    ui->startRecordingButton->setEnabled(true);
}

