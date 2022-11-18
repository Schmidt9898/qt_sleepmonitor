#include "cameraclass.h"

CameraClass::CameraClass()
{

}

CameraClass::~CameraClass()
{
    camPtr->DeInit();
    camList.Clear();
    system->ReleaseInstance();
    //delete this;
}

int CameraClass::GetCamera()
{
    int result = 0;
    try
    {
        // Retrieve singleton reference to system object
        system = System::GetInstance();

        // Retrieve list of cameras from the system
        camList = system->GetCameras();
        unsigned int numCameras = camList.GetSize();

        std::cout << "Number of cameras detected: " << numCameras << "\n\n";

        // Finish if there are no cameras
        if (numCameras == 0)
        {
            std::cout << "Camera not connected!" << "\n";
            return 1;
        }

        std::cout << "Camera detected. Returning pointer..." << "\n";

        camPtr = camList.GetByIndex(0);
        result = result | InitCamera();

        //      NOT SURE IF NEEDED, need more testing
        // Clear camera list before releasing system
        //std::cout << "clearing list\n";
        //camList.Clear();

        std::cout << "returning\n";
        return result;
    }
    catch(Spinnaker::Exception& e)
    {
        std::cout << "Error:" << e.what() << "\n";
        return -1;
    }
}

int CameraClass::InitCamera()
{
    int result = 0;
    std::cout << "Camera init started\n";
    try
    {
        camPtr->Init();

        INodeMap& nodeMap = camPtr->GetNodeMap();

        // Configure camera settings (fps, acquisition mode, etc.)
        //result = result | ConfigureCamera(nodeMap);
        ConfigureCamera(nodeMap);

        //camPtr->DeInit();

        std::cout << "Camera init ended\n";

        return result;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error at initializing: " << e.what() << "\n";
        return -1;
    }

}

void CameraClass::StartRecording(int recordLength, int numParts)
{
    std::cout << "Recording started\n";
    try
    {
        for (int curPart = 1; curPart <= numParts; curPart++)
        {
        if (!camPtr->IsInitialized()) camPtr->Init();

        // Calculate required number of frames for 1 video file
        const int numImages = (FRAMERATE * recordLength) + 24;

        // Begin acquiring images
        CameraClass::camPtr->BeginAcquisition();
        std::cout << "Acquisition started..." << "\n" << "\n";

        // *** NOTES ***
        // By default, if no specific color processing algorithm is set, the image
        // processor will default to NEAREST_NEIGHBOR method.
        ImageProcessor processor;
        processor.SetColorProcessing(HQ_LINEAR);

        SpinVideo video;
        //const unsigned int k_videoFileSize_MB = 4096;
        //video.SetMaximumFileSize(k_videoFileSize_MB);

        Video::H264Option option;
        option.frameRate = FRAMERATE;
        option.bitrate = BITRATE;
        option.height = HEIGHT;
        option.width = WIDTH;

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point elapsed;

        std::string videoFilename = "Video_" + std::to_string(curPart) + "_" + std::to_string(time(0));
        video.Open(videoFilename.c_str(), option);

        // *** IMPORTAN NOTE TO SELF ***
        // The first 24 frames in the MP4 file won't be buffered
        for (int imageCnt = 1; imageCnt <= numImages; imageCnt++)
        {
            try
            {

                // Retrieve the next received image
                ImagePtr pResultImage = CameraClass::camPtr->GetNextImage(1000);

                if (pResultImage->IsIncomplete())
                {
                    //SetConsoleTextAttribute(hConsole, 12);
                    std::cout << "Image "<< imageCnt << " is incomplete with image status " << pResultImage->GetImageStatus() << "..." << "\n"
                        << "\n";
                }
                else
                {
                    //SetConsoleTextAttribute(hConsole, 10);

                    std::cout << "------------------------" << "\n";
                    std::cout << "Grabbed image " << imageCnt << "/" << numImages << "\n";

                    cv::Mat cvimg = cv::Mat(480, 640, CV_16UC1, pResultImage->GetData(), pResultImage->GetStride());

                    cvimg = cvimg - 23800;
                    cvimg = cvimg * 50;

                    // Deep copy image into mp4 file
                    video.Append(processor.Convert(pResultImage, PixelFormat_Mono8));
                    std::cout << "Appended image " << imageCnt << "/" << numImages << " to part:" << curPart << "\n";

                    // Release image
                    pResultImage->Release();

                    elapsed = std::chrono::steady_clock::now();
                    std::cout << "Recording time elapsed: "
                              << std::chrono::duration_cast<std::chrono::seconds>(elapsed - begin).count()
                              << "s" << "\n";

                    //int processPercent = imageCnt*100/numImages;
                    //progressBar->setValue(processPercent);

                    std::cout << "------------------------" << "\n";
                }
            }

            catch (Spinnaker::Exception& e)
            {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
        video.Close();

        //const char* avi_filename = videoFilename.c_str() + '.avi';
        //const char* mp4_filename = videoFilename.c_str() + '.mp4';
        //rename(avi_filename, mp4_filename);

        std::cout << "\n" << "Video saved at " << videoFilename << ".avi" << "\n";

        // End acquisition
        CameraClass::camPtr->EndAcquisition();
        CameraClass::camPtr->DeInit();
        }

    std::cout << "Recording ended\n";
    return;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error at recording: " << e.what() << "\n";
    }

}

int CameraClass::ConfigureCamera(INodeMap& nodeMap)
{
    std::cout << "Camera config started\n";
    try
    {
        std::cout << "\n" << "\n" << "\t*** CONFIGURING CAMERA SETTINGS ***" << "\n";
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            std::cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << "\n" << "\n";
            return 1;
        }

        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            std::cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << "\n"
                << "\n";
            return 1;
        }

        int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
        std::cout << "----- Acquisition mode set to continuous... -----" << "\n";

        CFloatPtr ptrFramerate = nodeMap.GetNode("AcquisitionFrameRate");
        ptrFramerate->SetValue(FRAMERATE);
        std::cout << "----- Acquisition framerate set to 10 fps -----" << "\n";

        std::cout << "Camera config ended\n";
        return 0;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error at configuration: " << e.what() << "\n";
        return -1;
    }

}

//void PrintDeviceInfo(INodeMap& nodeMap){}
