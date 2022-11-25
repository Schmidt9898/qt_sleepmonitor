#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#define FRAMERATE 10
#define BITRATE 10000000
#define HEIGHT 480
#define WIDTH 640

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Spinnaker.h"
#include "SpinVideo.h"
#include <iostream>
#include <string>
#include <chrono>
#include <QProgressBar>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;


#include "qobjectdefs.h"
class CameraClass
{

public:
    CameraClass();
    ~CameraClass();

    /* Functions */
    int ConfigureCamera(INodeMap& nodeMap);
    void StartRecording(int recordLength, int numParts);
    int InitCamera();
    int GetCamera();
    //void GetSingleImage();

    /* Variables */
    bool isRecording = false;
    int offset = 23800;
    int gain = 50;

    CameraPtr camPtr;
    SystemPtr system;
    CameraList camList;


private:

signals:

private slots:

};

#endif // CAMERACLASS_H
