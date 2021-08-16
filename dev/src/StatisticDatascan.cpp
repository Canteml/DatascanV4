/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : StatisticDatascan.h
# Author      : Thierry Lechevallier
# Date        : 22/10/2020
#
# ==========================================================================*/

#include "StatisticDatascan.h"

StatisticDatascan * StatisticDatascan::m_pMyInstance = nullptr;

StatisticDatascan::StatisticDatascan()
{
    m_seDatascanStart = nullptr;
    m_seDistOutAcq = nullptr;
    m_seDistInAcq = nullptr;
    m_seFileCsv = nullptr;
    m_seUskeyAcq = nullptr;
    m_seDecoPic = nullptr;
    m_seDecoUsk = nullptr;
    m_stDatascanExecution = nullptr;
    m_stAcquisition = nullptr;
}

void StatisticDatascan::deleteStats()
{
    StatisticManager::deleteStats();
    m_seDatascanStart = nullptr;
    m_seDistOutAcq = nullptr;
    m_seDistInAcq = nullptr;
    m_seFileCsv = nullptr;
    m_seUskeyAcq = nullptr;
    m_seDecoPic = nullptr;
    m_seDecoUsk = nullptr;
    m_stDatascanExecution = nullptr;
    m_stAcquisition = nullptr;
}

StatisticDatascan *StatisticDatascan::makeInstance()
{
    if( nullptr == m_pMyInstance ) {
        m_pMyInstance = new StatisticDatascan();
    } else {
        Logger::logError("StatisticDatascan::makeInstance : instance already exists");
    }
    return m_pMyInstance;
}

StatisticDatascan *StatisticDatascan::getInstance()
{
    return m_pMyInstance;
}

StatisticDatascan::~StatisticDatascan()
{
    m_pMyInstance = nullptr;
}

void StatisticDatascan::makeStatistics(void)
{
    m_seDatascanStart = new StatEvent(tr("Ouvertures de Datascan"), "");
    m_seDistOutAcq = new StatEvent(tr("Distance parcourue hors acquisition"), "cm");
    m_seDistInAcq = new StatEvent(tr("Distance parcourue pendant les acquisitions"), "cm");
    m_seFileCsv = new StatEvent(tr("Nombre de CSV enregistrés"), "");
    m_seUskeyAcq = new StatEvent(tr("Nombre d'acquisitions du capteur"), "");
    m_seDecoPic = new StatEvent(tr("Nombre de déconnexions du PIC"), "");
    m_seDecoUsk = new StatEvent(tr("Nombre de déconnexions du capteur"), "");

    m_stDatascanExecution = new StatTime(tr("Temps d'exécution de Datascan"));
    m_stAcquisition = new StatTime(tr("Temps passé en acquisition"));

    addStat(m_seDatascanStart);
    addStat(m_seDistOutAcq);
    addStat(m_seDistInAcq);
    addStat(m_seFileCsv);
    addStat(m_seUskeyAcq);
    addStat(m_seDecoPic);
    addStat(m_seDecoUsk);

    addStat(m_stDatascanExecution);
    addStat(m_stAcquisition);
}

// -- Statistics
void StatisticDatascan::incDatascanStart()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seDatascanStart ) {
        m_pMyInstance->m_seDatascanStart->eventOccur();
    }
}

void StatisticDatascan::incDistOutAcq(qint64 p_value)
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seDistOutAcq ) {
        m_pMyInstance->m_seDistOutAcq->eventOccur(p_value);
    }
}

void StatisticDatascan::incDistInAcq(qint64 p_value)
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seDistInAcq ) {
        m_pMyInstance->m_seDistInAcq->eventOccur(p_value);
    }
}

void StatisticDatascan::incFileCsv()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seFileCsv ) {
        m_pMyInstance->m_seFileCsv->eventOccur();
    }
}

void StatisticDatascan::incUskeyAcq()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seUskeyAcq ) {
        m_pMyInstance->m_seUskeyAcq->eventOccur();
    }
}

void StatisticDatascan::incDecoPic()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seDecoPic ) {
        m_pMyInstance->m_seDecoPic->eventOccur();
    }
}

void StatisticDatascan::incDecoUsk()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_seDecoUsk ) {
        m_pMyInstance->m_seDecoUsk->eventOccur();
    }
}

void StatisticDatascan::startDatascanExecution()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_stDatascanExecution ) {
        m_pMyInstance->m_stDatascanExecution->enableTimer();
    }
}

void StatisticDatascan::stopDatascanExecution()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_stDatascanExecution ) {
        m_pMyInstance->m_stDatascanExecution->disableTimer();
    }
}

void StatisticDatascan::startAcquisition()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_stAcquisition ) {
        m_pMyInstance->m_stAcquisition->enableTimer();
    }
}

void StatisticDatascan::stopAcquisition()
{
    if( nullptr != m_pMyInstance && m_pMyInstance->m_stAcquisition ) {
        m_pMyInstance->m_stAcquisition->disableTimer();
    }
}

