#include "CoreApp.h"

#include <QMessageBox>
#include <QSharedMemory>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Guarantee that any child process this app spawns - in particular
    // ExeUSB2X86.exe, spawned indirectly by ap_int_usb.dll's "RunExeX32"
    // command (src/uskey/UsKeyDriver.cpp) - is terminated by Windows itself
    // the moment this process ends, no matter how: normal exit, a crash, or
    // a forced kill from Task Manager. Child processes inherit job membership,
    // so assigning this process to the job before any child is spawned covers
    // every helper the DLL creates. The handle is intentionally never closed:
    // Windows reclaims it on process exit, triggering kill-on-close.
    {
        HANDLE hJob = CreateJobObjectW(nullptr, nullptr);
        if (hJob) {
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
            jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
            AssignProcessToJobObject(hJob, GetCurrentProcess());
        }
    }
#endif

    CoreApp app;

    // Check if there is no other Datascan V4 instance
    /* Creation of the shared memory zone identified by a character string
     *  We will use a GUID to have a unique identifier so that there is no possible conflict with other applications
     */
    QSharedMemory sharedMemory("{57984FCD-F978-4768-9D05-F898BC58D159}"); // see the App ino setup for the correct ID to use
    // Check when creating this memory zone if it exists
    if(false==sharedMemory.create(sizeof(int)))
    {
        return 55; // Close the reduncy application
    }

    if(QString(APP_VERSION).contains("ENG"))
    {
        return app.execUiQml_EnglishLanguage(argc, argv);
    }
    else if(QString(APP_VERSION).contains("ES"))
    {
        return app.execUiQml_EspanolLanguage(argc, argv);
    }
    else
    {
         return app.execUiQml(argc, argv);
    }

}