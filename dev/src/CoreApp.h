/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CoreApp.h
# Author      : Thierry Lechevallier
# Date        : 04/06/2020
#
# ==========================================================================*/

#pragma once

// Internal includes
#include <coreUi/CoreUiQml.h>
class StatisticApp;
class VectorManager;
class VectorStd;
class ScanManager;

class CoreApp : public CoreUiQml
{
public:
    CoreApp();
    virtual ~CoreApp();


    // -- Core
    virtual QString getApplicationName() const override;
    virtual QString getVersion() const override;

    // -- state machine
    virtual bool initApp() override;
    virtual bool startApp() override;
    virtual void stopApp() override;
    virtual void deleteApp() override;

    // -- Qml

    virtual bool initQmlType() override;
    virtual bool initQmlContext() override;
    virtual bool initQmlPath() override;
    virtual void initCppFromQml() override;

private :

    // managers
    StatisticApp * m_pStatisticApp;
    VectorManager * m_pVectorManager;
    VectorStd * m_pVectorStd;

    ScanManager* m_pScanManager;

};

