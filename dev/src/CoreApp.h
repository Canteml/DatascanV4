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
#include "SoftwareLicenceChecker.h"
class StatisticApp;
class VectorManager;
class VectorStd;
class ScanManager;
class GlobalManager;
class ExamManager;
class JoystickManager;
class CameraManager;
class ExamPipeManager;
class ClipboardAdapter;
class MepExporter;
class ProductLimiter;
class UsKeyController;
class AltiscanController;
class EncoderController;
class SerialPortRouter;

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

    virtual QString getWindowTitle(void) const override;

protected :
    bool eventFilter(QObject*, QEvent*) override;   ///< key catcher

private :
    bool useOpenGlRender() const override;

    // managers
    StatisticApp * m_pStatisticApp;
    VectorManager * m_pVectorManager;
    VectorStd * m_pVectorStd;

    ScanManager* m_pScanManager;
    ExamManager* m_pExamManager;
    GlobalManager* m_pGlobalManager;
    JoystickManager* m_pJoystickManager;
    CameraManager* m_pCameraManager;
    ExamPipeManager* m_pExamPipeManager;

    MepExporter* m_pMepExporter;

    ClipboardAdapter* m_pClipboardAdapter;

    ProductLimiter* m_pProductLimiter;

    UsKeyController* m_pUsKeyController;

    SoftwareLicenceChecker m_licence;

    AltiscanController* m_pAltiscanController = nullptr;
    EncoderController* m_pEncoderController = nullptr;
    SerialPortRouter* m_pSerialPortRouter = nullptr;
};

