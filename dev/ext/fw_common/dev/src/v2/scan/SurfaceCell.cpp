/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : SurfaceCell.cpp
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include "SurfaceCell.h"
#include <v2/scan/Location1d.h>
#include <v2/scan/AScan.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncTemperatureSensor.h>
#include <v2/vector/VectorFuncHumiditySensor.h>

// -- Qt Includes

SurfaceCell::SurfaceCell(Location1d* p_pLocation1d) :Loggable("SurfaceCell")
{
    m_pLocation1d = p_pLocation1d;
    m_pAScanReference = nullptr;
}

SurfaceCell::~SurfaceCell()
{
    m_pAScanReference = nullptr;
}

bool SurfaceCell::pushAScan(AScan* p_pAScan)
{
    bool bKeeptAScan = true;
    lockData();
    // Select one algo :
    // -- ALGO LAST VALUE : keep only the last ASCAN
    if( m_pAScanReference != nullptr ) {
        m_pAScanReference->m_cellsCount--;
        if( 0 > m_pAScanReference->m_cellsCount ) {
            fLogError("Invalid cell count for AScan !");
        } else if( 0 == m_pAScanReference->m_cellsCount ) {
            m_pLocation1d->deleteStoredAScan(m_pAScanReference);
        }
    }
    m_pAScanReference = p_pAScan;
    m_pAScanReference->m_cellsCount++;

    // Collecting temperature and humidity from the current VectorStd to store them
    VectorStd* pCurrentVector = VectorManager::getInstance()->getVectorStd();

    // If the functions are not enabled, we set the variables to impossible values

    if( pCurrentVector->funcTemperatureSensor0()->getIsEnabled() ) {
        m_rTemperature = pCurrentVector->funcTemperatureSensor0()->getParamDTemperatureC() / 10.0;
    } else {
        m_rTemperature = -300.0; // -300° is impossible
    }

    if( pCurrentVector->funcHumiditySensor0()->getIsEnabled() ) {
        m_rHumidity = pCurrentVector->funcHumiditySensor0()->getParamDHumidityC() / 10.0;
    } else {
        m_rHumidity = -1.0; // -1% humidity is impossible
    }

    // -- ALGO MINIMAL : keep AScan with minimal measure

    unlockData();
    return bKeeptAScan;
}

AScan* SurfaceCell::getAScanReference() const
{
    return m_pAScanReference;
}

QString SurfaceCell::getTemperatureStr() const
{
    if( -273.15 < m_rTemperature ) {
        return QString::number(m_rTemperature);
    }
    return QString("-");
}

QString SurfaceCell::getHumidityStr() const
{
    if( 0 < m_rHumidity ) {
        return QString::number(m_rHumidity);
    }
    return QString("-");
}

void SurfaceCell::lockData()
{
    m_mutData.lock();
}

void SurfaceCell::unlockData()
{
    m_mutData.unlock();
}
