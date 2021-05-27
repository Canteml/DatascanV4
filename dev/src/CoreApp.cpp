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

// -- Qt Includes
#include <QQmlApplicationEngine>

// -- Internal includes
#include "CoreApp.h"
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/com/IntComMsg.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComLinkManager.h>
#include <v2/com/ComComponentManager.h>


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
    return "DatascanV2";
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
    m_pVectorManager = VectorManager::makeInstance();

    // -- init my managers
    // example : m_pStatisticApp->initManager();
    m_pVectorManager->init();

    // -- load settings of my managers
    // example : bRet &= m_pStatisticApp->loadSettings();

    return bRet;
}

bool CoreApp::startApp()
{
    bool bRet = true;

    // -- start my managers
    m_pVectorManager->makeVectorStd();
    m_pVectorStd = m_pVectorManager->getVectorStd();
    m_pVectorStd->setProdIdCompId(ProtMsg::PROD_PANDA_VOL3, ProtMsg::COMP_GROUND_CONTROL);
    m_pVectorStd->getLinkManager()->openSerial(28,115200);


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

    // -- stop my managers
    m_pVectorManager->quit();

    // -- core stop
    CoreUiQml::stopApp();
}

void CoreApp::deleteApp()
{
    // delete managers

    // -- core delete
    CoreUiQml::deleteApp();
}

// -- QML

bool CoreApp::initQmlType()
{
    // -- register my types to be used in QML :
    // example : qRegisterMetaType<vector*>("vector*");

    // -- declare C++ components instanciated from QML
    // exemple : qmlRegisterType<AScan>("ascan", 1, 0, "AScanItem");

    return true;
}

bool CoreApp::initQmlContext()
{
    // -- register my managers for QML use :
    // example : m_pQmlApplicationEngine->rootContext()->setContextProperty("vectorManager", m_vectorManager);
    return true;
}

bool CoreApp::initQmlPath()
{
    m_pQmlApplicationEngine->addImportPath(getDirAppQml());
    return true;
}
