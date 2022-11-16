#ifndef CONNECTTHREAD_H
#define CONNECTTHREAD_H

#include <QThread>
#include <QObject>
#include "cameraclass.h"

class ConnectThread : public QThread
{
    Q_OBJECT
public:
    explicit ConnectThread(QObject *parent = nullptr);
    void start(int recordLength, int numParts);

signals:
    void ConnectionFinished();
};

#endif // CONNECTTHREAD_H
