#include "CoreApp.h"

int main(int argc, char *argv[])
{
    CoreApp app;

    return app.execUiQml(argc, argv);
    //return app.execUiQml_EnglishLanguage(argc, argv);
    //return app.execUiQml_withSelectLanguage(argc, argv);
}
