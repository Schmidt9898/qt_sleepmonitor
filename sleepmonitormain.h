#ifndef SLEEPMONITORMAIN_H
#define SLEEPMONITORMAIN_H

#include <thread>
#include <QMainWindow>
#include <QProgressDialog>
#include <QTimer>

#include "cameraclass.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SleepMonitorMain; }
QT_END_NAMESPACE

class SleepMonitorMain : public QMainWindow
{
    Q_OBJECT

public:

    SleepMonitorMain(QWidget *parent = nullptr, CameraClass *cam = nullptr);
    ~SleepMonitorMain();

    /* Functions */
    bool GetConnecting(){bool val; connectionMutex.lock(); val = isConnecting; connectionMutex.unlock(); return val;}
    void SetConnecting(bool arg){ connectionMutex.lock(); isConnecting = arg; connectionMutex.unlock();}
    void GUIGetCamera(int *result);
    void DisplayPreview();
    void startRecording();

    void CameraTest();

    /* Variables */


private slots:
    void on_startRecordingButton_clicked();
    void on_recordTimeMin_valueChanged(int arg1);
    void on_recordTimeHour_valueChanged(int arg1);
    void on_connectButton_clicked();
    void on_recordParts_valueChanged(int arg1);

    void on_showPreviewButton_clicked();

    void on_hidePreviewButton_clicked();

    void on_gainSlider_valueChanged(int value);

    void on_offsetSlider_valueChanged(int value);

    void on_defaultButton_clicked();

public slots:
    void onConnectionFinished(int result);
    void onUpdateProgressbar(int value);
    void onRecordingEnded();

private:
    Ui::SleepMonitorMain *ui;
    CameraClass *camera;

    /* Functions */
    void UpdateDisplayedRecordTime(int recordTime);

    /* Variables */
    bool isConnected = false;
    bool isTimeNull = true;
    bool isPreviewActive = false;
    int recordMinute = 0;
    int recordHour = 0;
    int recordParts = 1;
    int recordTime = 0;

    int offset = 1;
    int gain = 1;
    int progress = 0;

    /* Threads */
    std::thread connectionThread;
    bool isConnecting = false;
    std::mutex connectionMutex;

    std::thread recordingThread;
    //bool isRecording = false;
    std::mutex recordingMutex;

    std::thread previewThread;
    bool isPreview = false;
    std::mutex previewMutex;

signals:
    void ConnectionFinished(int result);
    void UpdateProgressbar(int value);
    void RecordingEnded();

};
#endif // SLEEPMONITORMAIN_H
