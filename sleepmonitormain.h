#ifndef SLEEPMONITORMAIN_H
#define SLEEPMONITORMAIN_H

#include <thread>
#include <QMainWindow>

#include "recorder.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SleepMonitorMain; }
QT_END_NAMESPACE

class SleepMonitorMain : public QMainWindow
{
    Q_OBJECT

public:
    SleepMonitorMain(QWidget *parent = nullptr);
    ~SleepMonitorMain();

    std::thread recordingThread;

    bool isConnected = false;
    bool isTimeNull = true;
    int recordMinute = 0;
    int recordHour = 0;
    int recordParts = 1;
    int recordTime = 0;
    Spinnaker::CameraPtr camPtr = NULL;

private slots:
    void on_startRecordingButton_clicked();

    void on_recordTimeMin_valueChanged(int arg1);

    void on_recordTimeHour_valueChanged(int arg1);

    void on_connectButton_clicked();

    void on_recordParts_valueChanged(int arg1);

private:
    Ui::SleepMonitorMain *ui;

    void UpdateDisplayedRecordTime(int recordTime);
};
#endif // SLEEPMONITORMAIN_H
