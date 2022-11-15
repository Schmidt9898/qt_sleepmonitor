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
#include "waitwindow.h"
#include "qprogressbar.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;

int PrintDeviceInfo(INodeMap& nodeMap);
int ConfigureCamera(INodeMap& nodeMap);
int AcquireImages(CameraPtr pCam, int recordLength, int part, QProgressBar* progressBar);
int RecordTimeInput();
int RunSingleCamera(CameraPtr pCam, int secondsToRecord, int partsToRecord, QProgressBar* progressBar);

CameraPtr GetCamera();
