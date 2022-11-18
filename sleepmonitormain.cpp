#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"

#include "waitwindow.h"
#include "succeswindow.h"
#include "failwindow.h"

using namespace cv;

SleepMonitorMain::SleepMonitorMain(QWidget *parent, CameraClass *cam)
    : QMainWindow(parent), camera(cam), ui(new Ui::SleepMonitorMain)
{
    ui->setupUi(this);
    //ui->recordingProgressBar->hide();
}

SleepMonitorMain::~SleepMonitorMain()
{
    //camera->~CameraClass();
    delete ui;
}


void SleepMonitorMain::UpdateDisplayedRecordTime(int recordTime)
{
    ui->timeValueLabel->setText(QString::number(recordTime/60) + "h " + QString::number(recordTime%60) + "m");
}


void SleepMonitorMain::on_startRecordingButton_clicked()
{
    ui->startRecordingButton->setEnabled(false);
    WaitWindow waitwin;
    waitwin.show();
    camera->StartRecording(recordHour*60 + recordMinute, recordParts);
    waitwin.hide();
    ui->startRecordingButton->setEnabled(true);
}



void SleepMonitorMain::on_recordTimeMin_valueChanged(int arg1)
{
    recordMinute = arg1;
    SleepMonitorMain::UpdateDisplayedRecordTime((recordMinute + recordHour * 60) * recordParts);

    if (!recordMinute && !recordHour) isTimeNull = true;
    else isTimeNull = false;

    if(isConnected && !isTimeNull) ui->startRecordingButton->setEnabled(true);
    else ui->startRecordingButton->setEnabled(false);
}


void SleepMonitorMain::on_recordTimeHour_valueChanged(int arg1)
{
    recordHour = arg1;
    SleepMonitorMain::UpdateDisplayedRecordTime((recordMinute + recordHour * 60) * recordParts);

    if (recordMinute == 0 && recordHour == 0) isTimeNull = true;
    else isTimeNull = false;

    if(isConnected && !isTimeNull)    ui->startRecordingButton->setEnabled(true);
    else ui->startRecordingButton->setEnabled(false);
}


void SleepMonitorMain::on_recordParts_valueChanged(int arg1)
{
    recordParts = arg1;
    SleepMonitorMain::UpdateDisplayedRecordTime((recordMinute + recordHour * 60) * recordParts);
}


void SleepMonitorMain::on_connectButton_clicked()
{
    //cameraThread = std::thread(startCamera);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                                                // one of this will be useful someday
    //std::thread *th = new std::thread(GetCamera);
    //th->join();

    WaitWindow waitwin(this);
    ui->startRecordingButton->setEnabled(false);

    waitwin.show();
    int retval = camera->GetCamera();
    waitwin.close();

    std::cout << "returned value: " << retval << "\n";

    if (retval == 0)
    {
        isConnected = true;
        SuccesWindow resultwin;
        resultwin.exec();
        if(!isTimeNull) ui->startRecordingButton->setEnabled(true);
        ui->connectButton->setText("Connected");
        ui->connectButton->setEnabled(false);
        ui->connectButton->setFont(QFont("Arial", 12));
        ui->connectButton->setStyleSheet("color: green; font: bold");
    }
    else
    {
        isConnected = false;
        FailWindow resultwin;
        resultwin.exec();
        ui->connectButton->setStyleSheet("color: red; font: bold");
        ui->connectButton->setText("Could not connect\nPress to try again");
    }
}

void SleepMonitorMain::onConnectionFinished()
{
    std::cout << "signal received";
}
