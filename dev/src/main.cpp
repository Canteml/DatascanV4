#include "CoreApp.h"

#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    CoreApp app;


    // Check if there is no other Datascan V2 instance
    /* Creation of the shared memory zone identified by a character string
     *  We will use a GUID to have a unique identifier so that there is no possible conflict with other applications
     */
    QSharedMemory sharedMemory("{18A43CCC-3F2B-4F4B-8247-4EEFA600A762}"); // see the App ino setup for the correct ID to use
    // Check when creating this memory zone if it exists
    if(false==sharedMemory.create(sizeof(int)))
    {
        return 1; // Close the reduncy application
    }


    if(QString(APP_VERSION).contains("ENG"))
    {
        return app.execUiQml_EnglishLanguage(argc, argv);
    }
    else
    {
         return app.execUiQml(argc, argv);
    }


    //return app.execUiQml_withSelectLanguage(argc, argv);
}
