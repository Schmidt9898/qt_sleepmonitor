#include "sleepmonitormain.h"
#include <QApplication>

#define DEBUG
#ifdef DEBUG
    #include <windows.h>
#endif

int main(int argc, char *argv[])
{
#include <QApplication>
#include <QWidget>


    #ifdef DEBUG
    // detach from the current console window
    // if launched from a console window, that will still run waiting for the new console (below) to close
    // it is useful to detach from Qt Creator's <Application output> panel
    FreeConsole();

    // create a separate new console window
    AllocConsole();

    // attach the new console to this application's process
    AttachConsole(GetCurrentProcessId());

    // reopen the std I/O streams to redirect I/O to the new console
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    freopen("CON", "r", stdin);
#endif

    ///cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    ///// Retrieve singleton reference to system object
    ///SystemPtr system = System::GetInstance();

    ///// Retrieve list of cameras from the system
    ///CameraList camList = system->GetCameras();

    ///unsigned int numCameras = camList.GetSize();

    ///std::cout << "Number of cameras detected: " << numCameras << "\n" << "\n";

    ///// Finish if there are no cameras
    ///if (numCameras == 0)
    ///{
    ///    // Clear camera list before releasing system
    ///    camList.Clear();

    ///    // Release system
    ///    //system->ReleaseInstance();

    ///    std::cout << "Camera not connected!" << "\n";

    ///    CameraPtr camPtr = NULL;
    ///    return 0;
    ///}

    ///std::cout << "\n" << "Camera detected. Returning pointer..." << "..." << "\n";

    ///// Clear camera list before releasing system
    ///camList.Clear();
    ///// Release system
    /////system->ReleaseInstance();

    ///// Run example on each camera
    ///for (unsigned int i = 0; i < numCameras; i++)
    ///{
    ///    std::cout << "\n" << "Running example for camera " << i << "..." << "\n";

    ///    camList.GetByIndex(i);

    ///    std::cout << "Camera " << i << " example complete..." << "\n" << "\n";
    ///}
    ///CameraPtr camPtr = camList.GetByIndex(0);

    ///return 1;

    QApplication a(argc, argv);
    SleepMonitorMain w;
    w.show();
    return a.exec();
}

