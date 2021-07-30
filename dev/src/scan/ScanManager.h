/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AscanModel.h
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
class AScan;
class AScanGraph;


// -- Qt Includes
#include <QObject>
class QQmlApplicationEngine;

/**
 * @brief The ScanManager class
 *
 *  TODO : select source of ASCAN :
 *      - BSCAN if user click on it
 *      - VectorFuncAScanX
 */
class ScanManager : public QObject, public Loggable
{
public :
    ScanManager();
    ~ScanManager();

    void init();
    void initQmlObject(QObject *p_pQmlTopLevel);

    Q_INVOKABLE AScan* getAScanDisplayed();


private slots :
    void onNewAScanTreated(AScan*);

private :
    void onVectorProductChanged();

    AScan* m_pAScanDisplayed;
    AScanGraph* m_pAScanGraphMain;
    VectorStd * m_pVector;
};

