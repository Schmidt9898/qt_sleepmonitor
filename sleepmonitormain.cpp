#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"

#include "waitwindow.h"
#include "succeswindow.h"
#include "failwindow.h"

#include <thread>
#include <QProgressDialog>

using namespace cv;

SleepMonitorMain::SleepMonitorMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SleepMonitorMain)
{

    ui->setupUi(this);
    ui->recordingProgressBar->hide();
}

SleepMonitorMain::~SleepMonitorMain()
{
    delete ui;
}

void SleepMonitorMain::UpdateDisplayedRecordTime(int recordTime)
{
    ui->timeValueLabel->setText(QString::number(recordTime/60) + "h " + QString::number(recordTime%60) + "m");
}


void SleepMonitorMain::on_startRecordingButton_clicked()
{
    //ui->startRecordingButton->setEnabled(false);
    //recordingThread = std::thread(RunSingleCamera(camPtr, (recordHour * 60 + recordMinute) * 60, recordParts));
    //RunSingleCamera(camPtr, (recordHour * 60 + recordMinute) * 60, recordParts);

    ui->startRecordingButton->setEnabled(false);
    RunSingleCamera(camPtr, (recordHour * 60 + recordMinute) * 60, recordParts, ui->recordingProgressBar);
    ui->recordingProgressBar->show();
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


void SleepMonitorMain::on_connectButton_clicked()
{
    //cameraThread = std::thread(startCamera);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));

    //std::thread *th = new std::thread(GetCamera);
    //th->join();

    WaitWindow waitwin(this);
    waitwin.show();
    ui->startRecordingButton->setEnabled(false);

    Spinnaker::CameraPtr camPtr = GetCamera();
    std::cout << int(camPtr) << "\n";

    waitwin.close();

    if (camPtr)
    {
        isConnected = true;
        SuccesWindow resultwin;
        resultwin.exec();
        if(!isTimeNull) ui->startRecordingButton->setEnabled(true);

    }
    else
    {
        isConnected = false;
        FailWindow resultwin;
        resultwin.exec();
        ui->startRecordingButton->setEnabled(false);
    }


}


void SleepMonitorMain::on_recordParts_valueChanged(int arg1)
{
    recordParts = arg1;
    SleepMonitorMain::UpdateDisplayedRecordTime((recordMinute + recordHour * 60) * recordParts);
}

