/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : StatisticApp.h
# Author      : Thierry Lechevallier
# Date        : 22/10/2020
#
# ==========================================================================*/

//#include "StatisticApp.h"

//StatisticApp * StatisticApp::m_pMyInstance = nullptr;

//StatisticApp::StatisticApp()
//{
//    m_seAppStart = nullptr;
//    m_stAppExecution = nullptr;
//}

//void StatisticApp::deleteStats()
//{
//    StatisticManager::deleteStats();
//    m_seAppStart = nullptr;
//    m_stAppExecution = nullptr;
//}

//StatisticApp *StatisticApp::makeInstance()
//{
//    if( nullptr == m_pMyInstance ) {
//        m_pMyInstance = new StatisticApp();
//    } else {
//        Logger::logError("StatisticApp::makeInstance : instance already exists");
//    }
//    return m_pMyInstance;
//}

//StatisticApp *StatisticApp::getInstance()
//{
//    return m_pMyInstance;
//}

//StatisticApp::~StatisticApp()
//{
//    m_pMyInstance = nullptr;
//}

//void StatisticApp::makeStatistics(void)
//{
//    m_seAppStart = new StatEvent(tr("Ouvertures de App"), "");
//    m_stAppExecution = new StatTime(tr("Temps d'exécution de App"));

//    addStat(m_seAppStart);
//    addStat(m_stAppExecution);
//}

//// -- Statistics
//void StatisticApp::incAppStart()
//{
//    if( nullptr != m_pMyInstance && m_pMyInstance->m_seAppStart ) {
//        m_pMyInstance->m_seAppStart->eventOccur();
//    }
//}

//void StatisticApp::startAppExecution()
//{
//    if( nullptr != m_pMyInstance && m_pMyInstance->m_stAppExecution ) {
//        m_pMyInstance->m_stAppExecution->enableTimer();
//    }
//}

//void StatisticApp::stopAppExecution()
//{
//    if( nullptr != m_pMyInstance && m_pMyInstance->m_stAppExecution ) {
//        m_pMyInstance->m_stAppExecution->disableTimer();
//    }
//}
