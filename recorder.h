#ifndef RECORDER_H
#define RECORDER_H

#endif // RECORDER_H

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

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;

int PrintDeviceInfo(INodeMap& nodeMap);
int ConfigureCamera(INodeMap& nodeMap, INodeMap& nodeMapTLDevice);
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, int recordLength, int part);
int RecordTimeInput();
int RunSingleCamera(CameraPtr pCam);

CameraPtr GetCamera();
