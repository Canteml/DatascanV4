#include "CoreApp.h"

int main(int argc, char *argv[])
{
    CoreApp app;

    //return app.execUiQml_(argc, argv);
    return app.execUiQml_withSelectLanguage(argc, argv);
}
