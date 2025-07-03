#ifndef TESTCOREUIQML_H
#define TESTCOREUIQML_H

// Internal includes
#include <coreUi/CoreUiQml.h>

class TestCoreUiQml : public CoreUiQml
{
public:
    TestCoreUiQml();
    ~TestCoreUiQml();


    virtual QString getApplicationName() const;
};

#endif // TESTCOREUIQML_H
