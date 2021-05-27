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

#pragma once

#include <log/StatisticManager.h>

class StatisticDatascan : public StatisticManager
{
public:

    static StatisticDatascan * makeInstance();
    static StatisticDatascan * getInstance();
    ~StatisticDatascan();



    // -- Statistics
    static void incDatascanStart();
    static void incDistOutAcq(qint64 p_value);
    static void incDistInAcq(qint64 p_value);
    static void incFileCsv();
    static void incUskeyAcq();
    static void incDecoPic();
    static void incDecoUsk();

    static void startDatascanExecution();
    static void stopDatascanExecution();
    static void startAcquisition();
    static void stopAcquisition();

protected :

    StatisticDatascan();

    // Behaviour
    void deleteStats() override;
    void makeStatistics(void) override;


    StatEvent * m_seDatascanStart;  ///< index 0
    StatEvent * m_seDistOutAcq;     ///< index 1
    StatEvent * m_seDistInAcq;
    StatEvent * m_seFileCsv;
    StatEvent * m_seUskeyAcq;
    StatEvent * m_seDecoPic;
    StatEvent * m_seDecoUsk;        ///< index 6

    StatTime * m_stDatascanExecution;   ///< index 7
    StatTime * m_stAcquisition;         ///< index 8

    static StatisticDatascan * m_pMyInstance;
};

