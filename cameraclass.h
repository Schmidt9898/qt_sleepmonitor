#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#define FRAMERATE 10
#define BITRATE 10000000
#define HEIGHT 480
#define WIDTH 640

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "Spinnaker.h"
#include "SpinVideo.h"
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include "qprogressbar.h"

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

    static void RunCamera(int recordLength, int numParts);
    static CameraPtr GetCamera();
    static CameraPtr getCamPtr() { return camPtr; }

    /* Functions */
    //static void PrintDeviceInfo(INodeMap& nodeMap);
    static void ConfigureCamera(INodeMap& nodeMap);
    static void StartRecording(int recordLength, int numParts);
    static void InitCamera();

    /* Variables */
    static CameraPtr camPtr;
    //static INodeMap& nodeMapTLDevice;
    //static INodeMap& nodeMap;

private:

signals:

private slots:

};

#endif // CAMERACLASS_H
