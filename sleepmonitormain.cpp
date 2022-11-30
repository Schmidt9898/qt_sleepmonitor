#include "sleepmonitormain.h"
#include "ui_sleepmonitormain.h"


using namespace cv;

SleepMonitorMain::SleepMonitorMain(QWidget *parent, CameraClass *cam)
    : QMainWindow(parent), ui(new Ui::SleepMonitorMain), camera(cam)
{
    ui->setupUi(this);

    ui->recordingFinishedLabel->hide();
    ui->recordingProgressBar->hide();
    ui->disconnectedLabel->hide();

    connect(this, &SleepMonitorMain::ConnectionFinished, this, &SleepMonitorMain::onConnectionFinished);
    connect(this, &SleepMonitorMain::CameraDisconnected, this, &SleepMonitorMain::onCameraDisconnected);

    connect(this, &SleepMonitorMain::RecordingEnded, this, &SleepMonitorMain::onRecordingEnded);
    connect(this, &SleepMonitorMain::RecordingStarted, this, &SleepMonitorMain::onRecordingStarted);

    connect(this, &SleepMonitorMain::PreviewEnded, this, &SleepMonitorMain::onPreviewEnded);
    connect(this, &SleepMonitorMain::PreviewStarted, this, &SleepMonitorMain::onPreviewStarted);
}

SleepMonitorMain::~SleepMonitorMain()
{
    //camera->~CameraClass();
    //delete ui;
    camera->isRecording = false;
    camera->isPreview = false;
    isClosing = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (statusCheckThread.joinable()) statusCheckThread.join();
    if (previewThread.joinable()) previewThread.join();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SleepMonitorMain::UpdateDisplayedRecordTime(int recordTime)
{
    ui->timeValueLabel->setText(QString::number(recordTime/60) + "h " + QString::number(recordTime%60) + "m");
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SleepMonitorMain::on_startRecordingButton_clicked()
{
    if (camera->isRecording) return;
    if (recordingThread.joinable()) recordingThread.join();

    progress = 0;
    ui->recordingFinishedLabel->hide();
    ui->recordingProgressBar->setValue(0);
    ui->recordingProgressBar->show();
    ui->startRecordingButton->setEnabled(false);
    ui->stopRecordingButton->setEnabled(true);

    recordingThread = std::thread(&SleepMonitorMain::startRecording, this);
}

void SleepMonitorMain::on_stopRecordingButton_clicked()
{
    camera->isRecording = false;
}

void SleepMonitorMain::startRecording()
{
    emit RecordingStarted();

    int recordSeconds = recordHour*60 + recordMinute; // recordHour*3600 + recordMinute * 60;
    camera->StartRecording(recordSeconds, recordParts);

    emit RecordingEnded();
    return;
}

void SleepMonitorMain::onRecordingStarted()
{
    //int recordSeconds = (recordHour * 60 + recordMinute + 2) * recordParts;
    //QTimer *timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, QOverload<>::of(&SleepMonitorMain::UpdateProgressbar));
    //timer->start(recordSeconds*10);
}

void SleepMonitorMain::onRecordingEnded()
{
    std::cout << "recording ended, gui got signal\n\n";
    ui->startRecordingButton->setEnabled(true);
    ui->stopRecordingButton->setEnabled(false);
    ui->recordingFinishedLabel->show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    if(isConnecting) return;
    if(statusCheckThread.joinable()) statusCheckThread.join();

    isConnecting = true;
    statusCheckThread = std::thread(&SleepMonitorMain::GUIGetCamera, this, &result);

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

        std::cout << "Connection finished. Camera is now accessible.\n";
        ui->connectButton->setEnabled(false);
        ui->connectButton->setFont(QFont("Arial", 12));
        ui->connectButton->setStyleSheet("color: green");

        ui->showPreviewButton->setEnabled(true);
        ui->disconnectedLabel->hide();


        if(statusCheckThread.joinable()) statusCheckThread.join();
        statusCheckThread = std::thread(&SleepMonitorMain::StatusCheck, this);
        //QTimer *timer = new QTimer(this);
        //connect(timer, &QTimer::timeout, this, QOverload<>::of(&SleepMonitorMain::ConnectionCheck));
        //timer->start(1000);
    }
    else
    {
        isConnected = false;
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Could not connect\nPress to try again");
        //ui->connectButton->setStyleSheet("color: red");

        std::cout << "Connection failed.\n";

        ui->showPreviewButton->setEnabled(false);
    }
}

void SleepMonitorMain::GUIGetCamera(int* result)
{
    *result = camera->GetCamera();
    isConnecting = false;
    emit ConnectionFinished(*result);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SleepMonitorMain::on_showPreviewButton_clicked()
{
    if(camera->isPreview) return;
    if(previewThread.joinable()) previewThread.join();

    previewThread = std::thread(&SleepMonitorMain::DisplayPreview, this);
    //SleepMonitorMain::CameraTest();
}

void SleepMonitorMain::on_hidePreviewButton_clicked()
{
    emit PreviewEnded();
}

void SleepMonitorMain::DisplayPreview()
{
    try
    {
        if (!camera->camPtr->IsInitialized()) camera->camPtr->Init();
        if (!camera->camPtr->IsStreaming()) camera->camPtr->BeginAcquisition();
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error at preview: " << e.what() << "\n";
        return;
    }

    camera->isPreview = true;
    Spinnaker::ImagePtr spinImgPtr;
    Mat previewFrame;

    emit PreviewStarted();

    while (true)
    {
        try
        {
            spinImgPtr = camera->camPtr->GetNextImage(1000);
            previewFrame = cv::Mat(480, 640, CV_16UC1, spinImgPtr->GetData(), spinImgPtr->GetStride());
            previewFrame -= camera->offset;
            previewFrame *= camera->gain;
            ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(previewFrame.data,
                                                                previewFrame.cols,
                                                                previewFrame.rows,
                                                                previewFrame.step,
                                                                QImage::Format_Grayscale16)));
            spinImgPtr->Release();

            if (!camera->isPreview)
            {
                if (!camera->isRecording) camera->camPtr->EndAcquisition();
                break;
            }
        }
        catch(Spinnaker::Exception& e)
        {
            std::cout << "error at preview loop: " << e.what() << "\n";
            emit PreviewEnded();
            break;
        }
    }
}

void SleepMonitorMain::onPreviewStarted()
{
    camera->isPreview = true;
    ui->showPreviewButton->setEnabled(false);
    ui->hidePreviewButton->setEnabled(true);
    ui->imageLabel->show();
}

void SleepMonitorMain::onPreviewEnded()
{
    camera->isPreview = false;
    ui->showPreviewButton->setEnabled(true);
    ui->hidePreviewButton->setEnabled(false);
    ui->imageLabel->hide();
}

void SleepMonitorMain::CameraTest()
{
    camera->isPreview = true;

    cv::VideoCapture webcam(0);
    if (!webcam.isOpened())
    {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return;
    }

    cv::Mat frame;

    while (camera->isPreview)
    {
        webcam >> frame;

        cv::cvtColor(frame, frame, COLOR_BGR2GRAY);

        frame -= camera->offset;
        frame *= camera->gain;

        ui->imageLabel->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_Grayscale8)));

        if (cv::waitKey(10) == 27)
            break;
    }
    webcam.release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SleepMonitorMain::on_offsetSlider_valueChanged(int value)
{
    ui->offsetValueLabel->setText(QString::number(value));
    //this->offset = value;
    camera->offset = value;
}

void SleepMonitorMain::on_gainSlider_valueChanged(int value)
{
    ui->gainValueLabel->setText(QString::number(value));
    //this->gain = value;
    camera->gain = value;
}

void SleepMonitorMain::on_defaultButton_clicked()
{
    on_gainSlider_valueChanged(50);
    ui->gainSlider->setValue(50);

    on_offsetSlider_valueChanged(23750);
    ui->offsetSlider->setValue(23750);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SleepMonitorMain::onCameraDisconnected()
{
    ui->disconnectedLabel->show();
    isConnected = false;
    camera->isPreview = false;
    camera->isRecording = false;

    ui->connectButton->setEnabled(true);
    ui->connectButton->setText("Disconnected\nTry to connect again");
    ui->connectButton->setFont(QFont("Arial", 10));
    ui->connectButton->setStyleSheet("color: black");

    ui->startRecordingButton->setEnabled(false);
    ui->stopRecordingButton->setEnabled(false);
    ui->showPreviewButton->setEnabled(false);
    ui->hidePreviewButton->setEnabled(false);
}

void SleepMonitorMain::StatusCheck()
{
    while (!isClosing)
    {
        if (camera->system->GetCameras().GetSize() == 0)
        {
            emit CameraDisconnected();
            break;
        }
    }
}

