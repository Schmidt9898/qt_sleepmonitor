#include "connectthread.h"
#include <QtCore>

ConnectThread::ConnectThread(QObject *parent)
    : QThread{parent}
{

}

void ConnectThread::start(int recordLength, int numParts)
{
    //QMutex mutex;
    //mutex.lock();
    CameraClass::RunCamera(recordLength, numParts);
    this->sleep(10);
    //mutex.unlock();
    emit ConnectThread::ConnectionFinished();
}
