#ifndef SLEEPMONITORMAIN_H
#define SLEEPMONITORMAIN_H
#include <thread>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SleepMonitorMain; }
QT_END_NAMESPACE

class SleepMonitorMain : public QMainWindow
{
    Q_OBJECT

public:
    SleepMonitorMain(QWidget *parent = nullptr);
    ~SleepMonitorMain();

    //std::thread cameraThread;
    bool timeIsNull = true;
    int recordMinute = 0;
    int recordHour = 0;

private slots:
    void on_startRecordingButton_clicked();

    void on_recordTimeMin_valueChanged(int arg1);

    void on_recordTimeHour_valueChanged(int arg1);

    void on_connectButton_clicked();

private:
    Ui::SleepMonitorMain *ui;
};
#endif // SLEEPMONITORMAIN_H
