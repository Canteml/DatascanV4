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

// -- Internal includes
#include "CoreApp.h"
#include <v2/prot/ProtMsg.h>
#include <v2/com/IntComMsg.h>
#include <v2/com/ComLinkManager.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComDevice.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/vector/VectorFuncPump.h>
#include <scan/ScanManager.h>
#include <scan/AScanGraph.h>
#include <scan/BScanGraph.h>
#include "GlobalManager.h"
#include <v2/exam/ExamManager.h>
#include <v2/vector/VectorFuncPressureSensor.h>
#include <v2/vector/VectorFuncTemperatureSensor.h>
#include <v2/vector/VectorFuncHumiditySensor.h>
#include <v2/vector/VectorFuncLight.h>
#include <v2/vector/VectorFuncBattery.h>
#include <v2/vector/VectorFuncCleaning.h>
#include <v2/vector/VectorFuncEndStop.h>
#include <v2/vector/VectorFuncLicence.h>
#include <v2/vector/VectorFuncCompass.h>
#include "control/JoystickManager.h"
#include <v2/vector/VectorFuncMove.h>
#include "camera/CameraManager.h"
#include "exam/ExamPipeManager.h"
#include "exam/MepExporter.h"
#include "adapter/ClipboardAdapter.h"
#include "limit/ProductLimiter.h"
#include "camera/VlcQmlVideoSink.h"
#include <uskey/UsKeyController.h>
#include <altiscan/AltiscanController.h>
#include <encoder/EncoderController.h>
#include <encoder/SerialPortRouter.h>

#include <v2/camera/IpCamera.h>

// -- Qt Includes
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QMediaDevices>
#include <QCameraDevice>

#include <QSerialPortInfo>
#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "SoftwareLicenceChecker.h"

CoreApp::CoreApp() : CoreUiQml("CoreApp")
{
    m_pVectorManager = nullptr;
    m_pVectorStd = nullptr;
    m_pScanManager = nullptr;
    m_pExamManager = nullptr;
    m_pGlobalManager = nullptr;
    m_pJoystickManager = nullptr;
    m_pCameraManager = nullptr;
    m_pExamPipeManager = nullptr;
    m_pMepExporter = nullptr;
    m_pProductLimiter = nullptr;
    m_pUsKeyController = nullptr;
}

CoreApp::~CoreApp()
{

}

QString CoreApp::getApplicationName() const
{
    return "DatascanV4";
}

QString CoreApp::getVersion() const
{
    return QString(APP_VERSION);
}

// -- state machine

bool CoreApp::initApp()
{
    bool bRet = CoreUiQml::initApp();

    // -- make my managers
    m_pJoystickManager = JoystickManager::makeInstance();
    m_pVectorManager = VectorManager::makeInstance(this);
    m_pScanManager = ScanManager::makeInstance();
    m_pCameraManager = CameraManager::makeInstance();
    m_pExamPipeManager = ExamPipeManager::makeInstance();
    m_pExamManager = ExamManager::makeInstance();
    m_pGlobalManager = GlobalManager::makeInstance();

    // -- init my managers
    m_pVectorManager->init();
    m_pVectorStd = m_pVectorManager->getVectorStd();
    m_pScanManager->init();
    m_pGlobalManager->initManager();
    m_pExamManager->initManager();
    m_pJoystickManager->start();
    m_pCameraManager->start();
    m_pExamPipeManager->init();

    m_pAltiscanController = new AltiscanController(this);
    connect(m_pAltiscanController, &AltiscanController::evError, this,
            [this](const QString& msg) { fLogError(QString("[Altiscan] %1").arg(msg)); });

    m_pEncoderController = new EncoderController(this);
    connect(m_pEncoderController, &EncoderController::evError, this,
            [this](const QString& msg) { fLogError(QString("[Encoder] %1").arg(msg)); });
    m_pScanManager->setEncoderController(m_pEncoderController);
    m_pEncoderController->setLinkManager(m_pVectorStd->getLinkManager());
    connect(m_pEncoderController, &EncoderController::odometryChanged, m_pAltiscanController, &AltiscanController::setOdometry);
    connect(m_pEncoderController, &EncoderController::encoderReverseChanged, m_pAltiscanController, &AltiscanController::setEncoderReverse);
    m_pEncoderController->setPositionVector(m_pVectorStd);

    // -- "Port série de secours" (CentralPage1) : given to the Altiscan or to the
    // UT POLE encoder according to the selected product, see SerialPortRouter
    for (const QSerialPortInfo& p : QSerialPortInfo::availablePorts()) {
        fLogInfo(QString("[CoreApp] Available port: %1 | %2 | VID:0x%3 PID:0x%4")
                 .arg(p.portName(), p.description())
                 .arg(p.hasVendorIdentifier()  ? QString::number(p.vendorIdentifier(),  16).toUpper() : QString("----"))
                 .arg(p.hasProductIdentifier() ? QString::number(p.productIdentifier(), 16).toUpper() : QString("----")));
    }
    m_pSerialPortRouter = new SerialPortRouter(m_pVectorStd, m_pVectorStd->getLinkManager(),
                                               m_pAltiscanController, m_pEncoderController, this);

    // -- load settings of my managers
    bRet &= m_pVectorManager->loadSettings();
    bRet &= m_pExamManager->loadSettings();
    bRet &= m_pCameraManager->loadSettings();

    {
        const QList<QCameraDevice> testDevices = QMediaDevices::videoInputs();
        for (const QCameraDevice& d : testDevices) {
            fLogInfo(QString("CAMERA TEST:  - %1").arg(d.description()));
        }
    }

    m_pMepExporter = new MepExporter();
    m_pClipboardAdapter = new ClipboardAdapter(this);

    m_pProductLimiter = new ProductLimiter();
    m_pProductLimiter->loadLimitations();

    m_pUsKeyController = new UsKeyController(this);

    bRet &= m_licence.bInit();

    return bRet;
}

bool CoreApp::startApp()
{

    bool bRet = true;

    // -- start my managers
    if( ProtMsg::PROD_UNDEF == m_pVectorStd->getProdId() ) {
        fLogInfo("Make default Product : ALTISCAN");
        m_pVectorStd->setProdIdDeviceAd(ProtMsg::PROD_ALTISCAN_V2, ProtMsg::DEVICE_AD_MPC);
    }

    // -- give the backup serial port to the controller of the selected product
    m_pSerialPortRouter->apply();

    static QtMessageHandler s_previousHandler = nullptr;
    s_previousHandler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
        if (s_previousHandler) {
            s_previousHandler(type, ctx, msg);
        }

        static QFile logFile("C:/ProgramData/Roboplanet/DatascanV4/conf/qml_debug.txt");
        if (!logFile.isOpen()) {
            if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
                return;
            }
        }
        QString prefix;
        switch(type) {
        case QtDebugMsg:    prefix = "D"; break;
        case QtWarningMsg:  prefix = "W"; break;
        case QtCriticalMsg: prefix = "C"; break;
        case QtFatalMsg:    prefix = "F"; break;
        default:            prefix = "I"; break;
        }
        QTextStream(&logFile) << "[" << prefix << "] " << msg << "\n";
        logFile.flush();
    });

    // -- core start => open main.qml
    qputenv("QT_LOGGING_RULES", "qml=true;qt.qml.binding.removal.info=true");
    qputenv("QML_IMPORT_TRACE", "1");
    QLoggingCategory::setFilterRules("qml.debug=true");
    bRet &= CoreUiQml::startApp();

    if(1 == m_pProductLimiter->getEnabledProductsNames().size()) {
        QQmlProperty(m_pQmlTopLevel, "title").write(getWindowTitle() + m_pProductLimiter->getEnabledProductsNames().first());
    }

    return bRet;
}

void CoreApp::stopApp()
{
    m_pUsKeyController->deactivate();
    m_pAltiscanController->deactivate();
    m_pEncoderController->deactivate();
    m_pJoystickManager->stop();

    m_pJoystickManager->stop();
    m_pCameraManager->stop();

    // -- core stop
    CoreUiQml::stopApp();

    // -- stop my managers

}

#ifdef Q_OS_WIN
namespace {
    LONG WINAPI shutdownCrashFilter(EXCEPTION_POINTERS*)
    {
        ExitProcess(0);
    }
}
#endif

void CoreApp::deleteApp()
{
#ifdef Q_OS_WIN
    SetUnhandledExceptionFilter(shutdownCrashFilter);
#endif

    // Release the UsKey vendor DLL/device handle before the other managers.
    if(nullptr != m_pUsKeyController)
        delete m_pUsKeyController;
    m_pUsKeyController = nullptr;

    // delete managers
    if(nullptr != m_pScanManager)
        delete m_pScanManager;
    m_pScanManager = nullptr;
    if(nullptr != m_pExamManager)
        delete m_pExamManager;
    m_pExamManager = nullptr;
    if(nullptr != m_pGlobalManager)
        delete m_pGlobalManager;
    m_pGlobalManager = nullptr;

    if(nullptr != m_pVectorManager) {
        m_pVectorManager->quit();
        delete m_pVectorManager;
    }
    m_pVectorManager = nullptr;
    if(nullptr != m_pJoystickManager)
        delete m_pJoystickManager;
    m_pJoystickManager = nullptr;
    if(nullptr != m_pCameraManager)
        delete m_pCameraManager;
    m_pCameraManager = nullptr;
    if(nullptr != m_pExamPipeManager)
        delete m_pExamPipeManager;
    m_pExamPipeManager = nullptr;

    if(nullptr != m_pMepExporter)
        delete m_pMepExporter;
    m_pMepExporter = nullptr;

    if(nullptr != m_pClipboardAdapter)
        delete m_pClipboardAdapter;
    m_pClipboardAdapter = nullptr;

    if(nullptr != m_pProductLimiter)
        delete m_pProductLimiter;
    m_pProductLimiter = nullptr;

    // -- core delete
    CoreUiQml::deleteApp();
}

// -- QML

bool CoreApp::initQmlType()
{
    // -- register my types to be used in QML :
    // example : qRegisterMetaType<vector*>("vector*");
    qRegisterMetaType<ComDevice*>("ComDevice*");
    qRegisterMetaType<VectorFuncAScan*>("VectorFuncAScan*");
    qRegisterMetaType<VectorFuncPosition*>("VectorFuncPosition*");
    qRegisterMetaType<VectorFuncPump*>("VectorFuncPump*");
    qRegisterMetaType<VectorManager*>("VectorManager*");
    qRegisterMetaType<VectorStd*>("VectorStd*");
    qRegisterMetaType<ComLinkManager*>("ComLinkManager*");
    qRegisterMetaType<QAbstractListModel*>("QAbstractListModel*");
    qRegisterMetaType<ComDeviceManager*>("ComDeviceManager*");
    qRegisterMetaType<IntComMsg*>("IntComMsg*");
    qRegisterMetaType<ScanManager*>("ScanManager*");
    qRegisterMetaType<ExamManager*>("ExamManager*");
    qRegisterMetaType<GlobalManager*>("GlobalManager*");
    qRegisterMetaType<VectorFuncPressureSensor*>("VectorFuncPressureSensor*");
    qRegisterMetaType<VectorFuncHumiditySensor*>("VectorFuncHumiditySensor*");
    qRegisterMetaType<VectorFuncTemperatureSensor*>("VectorFuncTemperatureSensor*");
    qRegisterMetaType<VectorFuncLight*>("VectorFuncLight*");
    qRegisterMetaType<VectorFuncBattery*>("VectorFuncBattery*");
    qRegisterMetaType<VectorFuncCleaning*>("VectorFuncCleaning*");
    qRegisterMetaType<VectorFuncEndStop*>("VectorFuncEndStop");
    qRegisterMetaType<VectorFuncLicence*>("VectorFuncLicence*");
    qRegisterMetaType<VectorFuncCompass*>("VectorFuncCompass");
    qRegisterMetaType<JoystickManager*>("JoystickManager*");
    qRegisterMetaType<VectorFuncMove*>("VectorFuncMove*");
    qRegisterMetaType<CameraManager*>("CameraManager*");
    qRegisterMetaType<CameraRole*>("CameraRole*");
    qRegisterMetaType<CameraVideo*>("CameraVideo*");
    qRegisterMetaType<ExamPipeManager*>("ExamPipeManager*");
    qRegisterMetaType<MepExporter*>("MepExporter*");
    qRegisterMetaType<ClipboardAdapter*>("ClipboardAdapter*");
    qRegisterMetaType<ProductLimiter*>("ProductLimiter*");

    // -- declare C++ components instanciated from QML
    qmlRegisterType<AScanGraph>("AScanGraph", 1, 0, "AScanGraph");
    qmlRegisterType<BScanGraph>("BScanGraph", 1, 0, "BScanGraph");

    qmlRegisterType<VlcQmlVideoSink>("Camera", 1, 0, "VlcQmlVideoSink");

    return true;
}

bool CoreApp::initQmlContext()
{
    // -- register my managers for QML use :
    // example : m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_vectorManager);

    m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_pVectorManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("currentVector", m_pVectorStd);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("scanManager", m_pScanManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("examManager", m_pExamManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("globalManager", m_pGlobalManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("joystickManager", m_pJoystickManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("cameraManager", m_pCameraManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("examPipeManager", m_pExamPipeManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("mepExporter", m_pMepExporter);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("clipboard", m_pClipboardAdapter);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("products", m_pProductLimiter);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("encoderController", m_pEncoderController);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("serialPortRouter", m_pSerialPortRouter);

    m_pQmlApplicationEngine->rootContext()->setContextProperty(
        "modelCameraList", QVariant::fromValue(QStringList()));

    return true;
}

bool CoreApp::initQmlPath()
{
    m_pQmlApplicationEngine->addImportPath(getDirAppQml());
    QString s = getDirAppQml()+"/imports";
    m_pQmlApplicationEngine->addImportPath(s);
    return true;
}

void CoreApp::initCppFromQml()
{
    // -- map C++ object with QML item
    m_pScanManager->initQmlObject(m_pQmlTopLevel);    
    m_pCameraManager->initQmlObject(m_pQmlTopLevel);

    // -- US-Key standalone mode: activate after AScanGraph is connected
    connect(m_pUsKeyController, &UsKeyController::evError, this,
            [this](const QString& msg) { fLogError(QString("[UsKey] %1").arg(msg)); });

    if (m_pUsKeyController->activate(m_pVectorStd)) {
        fLogInfo("[CoreApp] US-Key standalone mode active");
        connect(m_pScanManager, &ScanManager::evSelectedFuncAScanChanged,
                m_pUsKeyController, [this](VectorFuncAScan*) {
                    m_pUsKeyController->syncToHardware();
                });
    } else {
        fLogInfo("[CoreApp] US-Key not available, running in normal mode");
    }

    m_pScanManager->loadSettings();

    fLogInfo("[CoreApp] initCppFromQml - ascanResetDefaultParams + UsKey sync");
    m_pScanManager->ascanResetDefaultParams();
}

QString CoreApp::getWindowTitle() const
{
    QString title = QString("%1 %2").arg(getApplicationName()).arg(getVersion());
    if(1 == m_pProductLimiter->getEnabledProductsNames().size()) {
        title += QString(" [%1]").arg(m_pProductLimiter->getEnabledProductsNames().first());
    }
    /*
    QString licenceMessage = m_licence.getTitleMessage();
    if(!licenceMessage.isEmpty()) {
        title += QString(" - %1").arg(licenceMessage);
    }
    */
    return title;
}

bool CoreApp::useOpenGlRender() const
{
    return true;
}

bool CoreApp::eventFilter(QObject * p_object, QEvent * p_event)
{
    if( QEvent::KeyPress == p_event->type() ) {
    }
    return CoreUiQml::eventFilter(p_object, p_event);
}
