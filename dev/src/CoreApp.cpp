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
#include "camera/Camera.h"
#include "exam/ExamPipeManager.h"
#include "exam/MepExporter.h"
#include "adapter/ClipboardAdapter.h"

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtQml/Qml.h>
#include <VLCQtQml/QmlSource.h>

// -- Qt Includes
#include <QQmlApplicationEngine>
#include <QQmlContext>

Q_DECLARE_METATYPE(VectorFuncAScan*)

CoreApp::CoreApp() : CoreUiQml("CoreApp")
{
    m_pStatisticApp = nullptr;
    m_pVectorManager = nullptr;
    m_pVectorStd = nullptr;
    m_pScanManager = nullptr;
    m_pExamManager = nullptr;
    m_pGlobalManager = nullptr;
    m_pJoystickManager = nullptr;
    m_pCameraManager = nullptr;
    m_pExamPipeManager = nullptr;
    m_pMepExporter = nullptr;
}


CoreApp::~CoreApp()
{

}

QString CoreApp::getApplicationName() const
{
    return "DataScanV2";
}

QString CoreApp::getVersion() const
{
    return QString(APP_VERSION); //QString("4.0.0");
}

// -- state machine

bool CoreApp::initApp()
{
    bool bRet = CoreUiQml::initApp();

    // -- make my managers
    // example : m_pStatisticApp = StatisticApp::makeInstance();    
    m_pJoystickManager = JoystickManager::makeInstance();
    m_pVectorManager = VectorManager::makeInstance(this);
    m_pScanManager = ScanManager::makeInstance();
    m_pCameraManager = CameraManager::makeInstance();
    m_pExamPipeManager = ExamPipeManager::makeInstance();

    m_pExamManager = ExamManager::makeInstance();
    m_pGlobalManager = GlobalManager::makeInstance();

    // -- init my managers
    // example : m_pStatisticApp->initManager();
    m_pVectorManager->init();
    m_pVectorStd = m_pVectorManager->getVectorStd();
    m_pScanManager->init();    
    m_pGlobalManager->initManager();
    m_pExamManager->initManager();
    m_pJoystickManager->start();
    m_pCameraManager->start();
    m_pExamPipeManager->init();

    // -- load settings of my managers
    // example : bRet &= m_pStatisticApp->loadSettings();
    bRet &= m_pVectorManager->loadSettings();
    bRet &= m_pExamManager->loadSettings();
    bRet &= m_pCameraManager->loadSettings();

    m_pMepExporter = new MepExporter();
    m_pClipboardAdapter = new ClipboardAdapter(this);

    return bRet;
}

bool CoreApp::startApp()
{
    bool bRet = true;

    // -- start my managers

    if( ProtMsg::PROD_UNDEF == m_pVectorStd->getProdId() ) {
        fLogInfo("Make default Product : ALTISCAN");
        m_pVectorStd->setProdIdDeviceAd(ProtMsg::PROD_ALTISCAN, ProtMsg::DEVICE_AD_MPC);
    }
//    ComDevice * pComp = m_pVectorStd->getDeviceManager()->getComp(0);
//    pComp->setLink()



    // -- core start => open main.qml
    bRet &= CoreUiQml::startApp();

    // -- Updates statistics :
//    StatisticApp::incAppStart();
//    StatisticApp::startAppExecution();

    return bRet;
}

void CoreApp::stopApp()
{
//    StatisticApp::stopAppExecution();
    m_pJoystickManager->stop();
    m_pCameraManager->stop();

    // -- core stop
    CoreUiQml::stopApp();

    // -- stop my managers


}

void CoreApp::deleteApp()
{
    // delete managers
    delete m_pScanManager;
    m_pScanManager = nullptr;
    delete m_pExamManager;
    m_pExamManager = nullptr;
    delete m_pGlobalManager;
    m_pGlobalManager = nullptr;

    m_pVectorManager->quit();
    delete m_pVectorManager;
    m_pVectorManager = nullptr;
    delete m_pJoystickManager;
    m_pJoystickManager = nullptr;
    delete m_pCameraManager;
    m_pCameraManager = nullptr;
    delete m_pExamPipeManager;
    m_pExamPipeManager = nullptr;

    delete m_pMepExporter;
    m_pMepExporter = nullptr;

    delete m_pClipboardAdapter;
    m_pClipboardAdapter = nullptr;

    delete m_pSource;
    delete m_pPlayer;
    delete m_pMedia;
    delete m_pInstance;

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

    // -- declare C++ components instanciated from QML
    // exemple : qmlRegisterType<AScan>("ascan", 1, 0, "AScanItem");
    qmlRegisterType<AScanGraph>("AScanGraph", 1, 0, "AScanGraph");
    qmlRegisterType<BScanGraph>("BScanGraph", 1, 0, "BScanGraph");

    //qRegisterMetaType<VlcMediaPlayer*>("MediaPlayer*");
    VlcQml::registerTypes();

    return true;
}

bool CoreApp::initQmlContext()
{
    // -- register my managers for QML use :
    // example : m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_vectorManager);

    QStringList args = QStringList(QString("--network-caching=200"));
    args.append("--clock-synchro=0");
    args.append("--clock-jitter=500");
    args.append("--rtsp-tcp");
    m_pInstance = new VlcInstance(args, this);
    m_pMedia = new VlcMedia(QString("rtsp://admin:RPje2022@192.168.0.54:554"), m_pInstance);
    //m_pMedia->setOption(QString("network-caching=50"));
    m_pPlayer = new VlcMediaPlayer(m_pInstance);
    m_pPlayer->open(m_pMedia);
    m_pSource = new VlcQmlSource(this);
    m_pSource->setPlayer(m_pPlayer);
    m_pPlayer->play();

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
    m_pQmlApplicationEngine->rootContext()->setContextProperty("mediaSource", m_pSource);

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

    // load graphical settings (wait QML instanciated)
    m_pScanManager->loadSettings();
}

bool CoreApp::useOpenGlRender() const
{
    return true;
}

bool CoreApp::eventFilter(QObject * p_object, QEvent * p_event)
{
    if( QEvent::KeyPress == p_event->type() ) {
//        QKeyEvent * kpe = (QKeyEvent *)p_event;
//        if( Qt::Key_Space == kpe->key() ) {
//            m_pVectorManager->getVectorStd()->emergencyStop();
//        }
    }
    return CoreUiQml::eventFilter(p_object, p_event);
}
