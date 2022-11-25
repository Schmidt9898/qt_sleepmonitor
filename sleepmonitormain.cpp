#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"


using namespace cv;

SleepMonitorMain::SleepMonitorMain(QWidget *parent, CameraClass *cam)
    : QMainWindow(parent), ui(new Ui::SleepMonitorMain), camera(cam)
{
    ui->setupUi(this);

    ui->recordingFinishedLabel->hide();
    //ui->recordingProgressBar->hide();

    connect(this, &SleepMonitorMain::ConnectionFinished, this, &SleepMonitorMain::onConnectionFinished);
    connect(this, &SleepMonitorMain::UpdateProgressbar, this, &SleepMonitorMain::onUpdateProgressbar);
    connect(this, &SleepMonitorMain::RecordingEnded, this, &SleepMonitorMain::onRecordingEnded);
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
    if (camera->isRecording) return;
    if (recordingThread.joinable()) recordingThread.join();

    ui->startRecordingButton->setEnabled(false);
    ui->stopRecordingButton->setEnabled(true);

    recordingThread = std::thread(&SleepMonitorMain::startRecording, this);

}


void SleepMonitorMain::startRecording()
{
    camera->StartRecording(recordHour*60 + recordMinute, recordParts);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    emit RecordingEnded();
    return;
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
    if (result != 0)
    {
        isConnected = true;
        if(!isTimeNull) ui->startRecordingButton->setEnabled(true);

        ui->connectButton->setText("Connected");
        ui->connectButton->setEnabled(false);
        ui->connectButton->setFont(QFont("Arial", 12));
        ui->connectButton->setStyleSheet("color: green");

        ui->showPreviewButton->setEnabled(true);
    }
    else
    {
        isConnected = false;
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Could not connect\nPress to try again");
        //ui->connectButton->setStyleSheet("color: red");

        ui->showPreviewButton->setEnabled(true);
    }
}


void SleepMonitorMain::onRecordingEnded()
{
    std::cout << "recording ended, gui got signal\n\n";
    ui->startRecordingButton->setEnabled(true);
    ui->stopRecordingButton->setEnabled(false);
    ui->recordingFinishedLabel->show();
}


void SleepMonitorMain::on_showPreviewButton_clicked()
{
    if(isPreview) return;
    if(previewThread.joinable()) previewThread.join();

    ui->imageLabel->show();
    ui->showPreviewButton->setEnabled(false);
    ui->hidePreviewButton->setEnabled(true);
    previewThread = std::thread(&SleepMonitorMain::CameraTest, this);
    //SleepMonitorMain::CameraTest();
}


void SleepMonitorMain::on_hidePreviewButton_clicked()
{
        ui->imageLabel->hide();
        ui->showPreviewButton->setEnabled(true);
        ui->hidePreviewButton->setEnabled(false);
        isPreview = false;
}


void SleepMonitorMain::DisplayPreview()
{
    isPreview = true;

    Spinnaker::ImagePtr spinImgPtr;
    Mat previewFrame;
    try
    {
        camera->camPtr->BeginAcquisition();
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error at preview: " << e.what() << "\n";
        return;
    }

    while (true)
    {
        spinImgPtr = camera->camPtr->GetNextImage(1000);
        previewFrame = cv::Mat(480, 640, CV_16UC1, spinImgPtr->GetData(), spinImgPtr->GetStride());
        previewFrame -= offset;
        previewFrame *= gain;
        ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(previewFrame.data,
                                                            previewFrame.cols,
                                                            previewFrame.rows,
                                                            previewFrame.step,
                                                            QImage::Format_Grayscale16)));
        spinImgPtr->Release();

        if (!isPreview)
        {
            camera->camPtr->EndAcquisition();
            break;
        }
    }
}


void SleepMonitorMain::CameraTest()
{
    isPreview = true;

    cv::VideoCapture camera(0);
    if (!camera.isOpened())
    {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return;
    }

    cv::Mat frame;


    while (isPreview)
    {
        camera >> frame;

        cv::cvtColor(frame, frame, COLOR_BGR2GRAY);

        frame -= offset;
        frame *= gain;

        ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8)));

        progress += 1;
        if (progress%10 == 0) emit UpdateProgressbar(progress/10);

        if (cv::waitKey(10) == 27)
            break;
    }
    camera.release();
}


void SleepMonitorMain::onUpdateProgressbar(int value)
{
    ui->recordingProgressBar->setValue(value);
}


void SleepMonitorMain::on_offsetSlider_valueChanged(int value)
{
    ui->offsetValueLabel->setText(QString::number(value));
    this->offset = value;
    camera->offset = value;
}


void SleepMonitorMain::on_gainSlider_valueChanged(int value)
{
    ui->gainValueLabel->setText(QString::number(value));
    this->gain = value;
    camera->gain = value;
}


void SleepMonitorMain::on_defaultButton_clicked()
{
    ui->gainSlider->setValue(1);
    ui->offsetSlider->setValue(0);
}

