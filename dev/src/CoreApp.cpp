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
#include <v2/vector/VectorFuncPosition.h>
#include <scan/ScanManager.h>
#include <scan/AScanGraph.h>

// -- Qt Includes
#include <QQmlApplicationEngine>
#include <QQmlContext>


CoreApp::CoreApp() : CoreUiQml("CoreApp")
{
    m_pStatisticApp = nullptr;
    m_pVectorManager = nullptr;
    m_pVectorStd = nullptr;
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
    m_pVectorManager = VectorManager::makeInstance(this);
    m_pScanManager = new ScanManager();

    // -- init my managers
    // example : m_pStatisticApp->initManager();
    m_pVectorManager->init();
    m_pVectorStd = m_pVectorManager->getVectorStd();
    m_pScanManager->init();

    // -- load settings of my managers
    // example : bRet &= m_pStatisticApp->loadSettings();
    bRet &= m_pVectorManager->loadSettings();

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

    // -- core stop
    CoreUiQml::stopApp();

    // -- stop my managers


}

void CoreApp::deleteApp()
{
    // delete managers
    m_pVectorManager->quit();

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
    qRegisterMetaType<VectorFuncAScan*>("VectorFuncPosition*");
    qRegisterMetaType<VectorManager*>("VectorManager*");
    qRegisterMetaType<VectorStd*>("VectorStd*");
    qRegisterMetaType<ComLinkManager*>("ComLinkManager*");
    qRegisterMetaType<QAbstractListModel*>("QAbstractListModel*");
    qRegisterMetaType<ComDeviceManager*>("ComDeviceManager*");
    qRegisterMetaType<IntComMsg*>("IntComMsg*");
    qRegisterMetaType<ScanManager*>("ScanManager*");

    // -- declare C++ components instanciated from QML
    // exemple : qmlRegisterType<AScan>("ascan", 1, 0, "AScanItem");
    qmlRegisterType<AScanGraph>("AScanGraph", 1, 0, "AScanGraph");

    return true;
}

bool CoreApp::initQmlContext()
{
    // -- register my managers for QML use :
    // example : m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_vectorManager);

    m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_pVectorManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("currentVector", m_pVectorStd);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("scanManager", m_pScanManager);

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
}

bool CoreApp::useOpenGlRender() const
{
    return true;
}
