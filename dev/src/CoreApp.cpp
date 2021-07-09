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

    // -- init my managers
    // example : m_pStatisticApp->initManager();
    m_pVectorManager->init();
    m_pVectorStd = m_pVectorManager->getVectorStd();

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
        fLogInfo("Make default Product : PANDA VOL3");
        m_pVectorStd->setProdIdDeviceAd(ProtMsg::PROD_PANDA_VOL3, ProtMsg::DEVICE_AD_MPC);
        m_pVectorStd->getLinkManager()->addSerial(29,921600, "Position");
        m_pVectorStd->getLinkManager()->addSerial(28,921600, "ASCAN 0");
        m_pVectorStd->getLinkManager()->addSerial(30,921600, "ASCAN 1");
        m_pVectorStd->getLinkManager()->addSerial(31,921600, "ASCAN 2");
        m_pVectorStd->getLinkManager()->addSerial(32,921600, "ASCAN 3");
    }
//    ComDevice * pComp = m_pVectorStd->getComponentManager()->getComp(0);
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


    // -- declare C++ components instanciated from QML
    // exemple : qmlRegisterType<AScan>("ascan", 1, 0, "AScanItem");

    return true;
}

bool CoreApp::initQmlContext()
{
    // -- register my managers for QML use :
    // example : m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_vectorManager);

    m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_pVectorManager);
    m_pQmlApplicationEngine->rootContext()->setContextProperty("currentVector", m_pVectorStd);

    return true;
}

bool CoreApp::initQmlPath()
{
    m_pQmlApplicationEngine->addImportPath(getDirAppQml());
    QString s = getDirAppQml()+"/imports";
    m_pQmlApplicationEngine->addImportPath(s);
    return true;
}
