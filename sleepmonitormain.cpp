#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"

#include "waitwindow.h"


using namespace cv;

SleepMonitorMain::SleepMonitorMain(QWidget *parent, CameraClass *cam)
    : QMainWindow(parent), ui(new Ui::SleepMonitorMain), camera(cam)
{
    ui->setupUi(this);
    //ui->recordingProgressBar->hide();

    connect(this, &SleepMonitorMain::ConnectionFinished, this, &SleepMonitorMain::onConnectionFinished);
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


void SleepMonitorMain::on_recordParts_valueChanged(int arg1){
    recordParts = arg1;
    SleepMonitorMain::UpdateDisplayedRecordTime((recordMinute + recordHour * 60) * recordParts);
}


void SleepMonitorMain::GUIGetCamera(int* result)
{
    *result = camera->GetCamera();
    std::cout << "\nGetCamera End\n";
    SetConnecting(false);
    emit ConnectionFinished(*result);
}


void SleepMonitorMain::on_connectButton_clicked()
{
    // one of this will be useful someday
    ///    cameraThread = std::thread(startCamera);
    ///    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ///
    ///    std::thread *th = new std::thread(GetCamera);
    ///    th->join();
    ///
    ///    QTimer::singleShot(2000, this, &SleepMonitorMain::onConnectionFinished);

    int result = 1;

    if(GetConnecting()) return;
    if(connectionThread.joinable()) connectionThread.join();

    SetConnecting(true);
    connectionThread = std::thread(&SleepMonitorMain::GUIGetCamera, this, &result);

    ui->connectButton->setEnabled(false);
    ui->connectButton->setText("Connecting...");
    //ui->connectButton->setStyleSheet("color: black");
}


void SleepMonitorMain::onConnectionFinished(int result)
{
    if (result == 0)
    {
        isConnected = true;
        if(!isTimeNull) ui->startRecordingButton->setEnabled(true);
        ui->connectButton->setText("Connected");
        ui->connectButton->setEnabled(false);
        ui->connectButton->setFont(QFont("Arial", 12));
        ui->connectButton->setStyleSheet("color: green");
    }
    else
    {
        isConnected = false;
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Could not connect\nPress to try again");
        //ui->connectButton->setStyleSheet("color: red");
    }
}

void SleepMonitorMain::on_showPreviewButton_clicked()
{
    if(isPreview) return;
    if(previewThread.joinable()) previewThread.join();

    ui->imageLabel->show();
    previewThread = std::thread(&SleepMonitorMain::CameraTest, this);
    //SleepMonitorMain::CameraTest();
}

void SleepMonitorMain::CameraTest()
{
    isPreview = true;

    cv::VideoCapture camera(0); // in linux check $ ls /dev/video0
    if (!camera.isOpened())
    {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return;
    }

    cv::Mat frame;


    while (isPreview) {
        camera >> frame;
        cv::cvtColor(frame, frame, COLOR_BGR2RGB);
        ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888)));

        if (cv::waitKey(10) == 27)
            break;
    }
    camera.release();
}


void SleepMonitorMain::on_hidePreviewButton_clicked()
{
        ui->imageLabel->hide();
        isPreview = false;
}

