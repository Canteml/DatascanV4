/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Surface1d.cpp
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include "Surface1d.h"
#include <v2/scan/Location1d.h>
#include <v2/scan/SurfaceCell.h>
#include <v2/scan/Zone1d.h>
#include <v2/scan/AScan.h>

// -- Qt Includes

const qreal Surface1d::DEFAULT_CELL_RESOLUTION_MM = 5;

Surface1d::Surface1d(Location1d* p_pLocation1d) : Loggable("Surface1d " + p_pLocation1d->getName())
{
    m_pLocation1d = p_pLocation1d;
    m_resolutionMm = DEFAULT_CELL_RESOLUTION_MM;
    m_surfaceStartMm = 0;
    m_nbCell = 0;

    m_positionMin = 0;
    m_positionMax = 0;
    m_measureMin = 0;
    m_measureMax = 0;
}

Surface1d::~Surface1d()
{
    clearData();
}

void Surface1d::clearData()
{
    lockData();
    m_nbCell = 0;
    for(int i=0; i<m_aCell.size(); i++) {
        delete m_aCell[i];
    }
    m_aCell.clear();
    m_aPositionM.clear();
    m_aMeasure1e.clear();
    m_aMeasureMe.clear();
    m_positionMin = 0;
    m_positionMax = 0;
    m_measureMin = 0;
    m_measureMax = 0;
    unlockData();
    emit evSurfaceChanged();
}

qint32 Surface1d::pushAScan(AScan* p_pAScan)
{
    qreal pos = p_pAScan->getAScanRawSample()->m_position.x();
    qint32 posIndex = ( pos - m_surfaceStartMm ) / m_resolutionMm;
    qint32 posIndexLeft = posIndex - 1;
    //fLogDebug1(QString("pos = %1 / index = %2 / start = %3 ").arg(pos).arg(posIndex).arg(m_surfaceStartMm));

    // -- Adjust size of arrays
    // first ASCAN ?

    lockData();
    bool bFirst = false;
    if( 0 == m_nbCell ) {
        m_surfaceStartMm = m_resolutionMm*((qint32)(pos)/((qint32)m_resolutionMm));
        m_aCell.append(new SurfaceCell(m_pLocation1d));
        m_aMeasure1e.append(0);
        m_aMeasureMe.append(0);
        m_aPositionM.append(m_surfaceStartMm/1000);
        m_nbCell = 1;
        bFirst = true;
    } else {
        // resize arrays if needed :
        if( posIndexLeft<0 ) {
            // insert cells at begin
            qint32 cellToInsert = -posIndexLeft;
            for(qint32 i=0; i < cellToInsert; i++) {
                m_aCell.insert(0, new SurfaceCell(m_pLocation1d));
                m_aMeasure1e.insert(0, 0);
                m_aMeasureMe.insert(0, 0);
                m_aPositionM.insert(0, (m_surfaceStartMm-(i+1) * m_resolutionMm)/1000);
                m_nbCell++;
            }
            //fLogDebug1(QString("cellToInsert = %1 / m_nbCell = %2").arg(cellToInsert).arg(m_nbCell));
            // cell position must be a multiple of 'resolution'
            m_surfaceStartMm = m_aPositionM[0]*1000;
            // updtate index
            if( abs(m_aPositionM[0]*1000 - m_surfaceStartMm) >=  m_resolutionMm ) {
                fLogWarning(QString("Start position is %1 but position array [0] is %2.").arg(m_surfaceStartMm).arg(m_aPositionM[0]*1000));
            }
        } else if( posIndex >= (qint32)m_nbCell) {
            // add cells at the end
            qint32 cellToAppend = 1+posIndex-m_nbCell;
            for(qint32 i=0; i < cellToAppend; i++) {
                m_aCell.append(new SurfaceCell(m_pLocation1d));
                m_aMeasure1e.append(0);
                m_aMeasureMe.append(0);
                m_aPositionM.append((m_surfaceStartMm + (m_nbCell) * m_resolutionMm)/1000);
                m_nbCell++;
            }
            //fLogDebug1(QString("cellToAppend = %1 / m_nbCell = %2").arg(cellToAppend).arg(m_nbCell));
            if( abs(pos - m_aPositionM[m_nbCell-1]*1000 ) >  m_resolutionMm ) {
                fLogWarning(QString("Last position array is %1 but appened position is %2.").arg(m_aPositionM[m_nbCell-1]*1000).arg(p_pAScan->getAScanRawSample()->m_position.x()));
            }
        }
    }

    /// -- Set cell with new ASCAN
    posIndex = ( pos - m_surfaceStartMm /*+ m_resolutionMm/2*/) / m_resolutionMm;
    // find Cell :
    posIndex = saveToCell(posIndex, p_pAScan, bFirst);

    unlockData();

    if( (-1 != posIndex) ) {
        emit evSurfaceChanged();
    }

    return posIndex;
}

qint32 Surface1d::saveToCell(qint32 p_posIndex, AScan* p_pAScan, bool p_bFirst)
{
    qint32 posIndex = p_posIndex;
    qreal pos = p_pAScan->getAScanRawSample()->m_position.x();
    if( posIndex >= (qint32)m_nbCell || posIndex < 0 ) {
        fLogError(QString("pos(%1) is out of array(%2)").arg(posIndex).arg(m_nbCell));
        posIndex = -1;
    } else {
        // give the ASCAN to the CELL
        SurfaceCell * pCell = m_aCell[posIndex];
        bool keepAScan = pCell->pushAScan(p_pAScan);

        if( keepAScan ) {
            // Update measures
            if( nullptr != pCell->getAScanReference() ) {
                qreal m1e, mMe;
                m1e = pCell->getAScanReference()->getAScanMeasure()->m_thicknessMm1Echo;
                mMe = pCell->getAScanReference()->getAScanMeasure()->m_thicknessMmMultiEcho;
                m_aMeasure1e[posIndex] = m1e;
                m_aMeasureMe[posIndex] = mMe;
                qreal mMax = qMax(m1e, mMe);
                qreal mMin = qMin(m1e, mMe);

                if( mMin < m_measureMin || p_bFirst ) {
                    m_measureMin = mMin;
                }
                if( mMax > m_measureMax || p_bFirst ) {
                    m_measureMax = mMax;
                }
                if( pos < m_positionMin || p_bFirst ) {
                    m_positionMin = pos;
                }
                if( pos > m_positionMax || p_bFirst ) {
                    m_positionMax = pos;
                }
            }
        } else {
            posIndex = -1;  // do not keept this ASCAN
        }
    }
    return posIndex;
}

qint32 Surface1d::positionToIndex(qreal p_posMm)
{
    qint32 posIndex = ( p_posMm - m_surfaceStartMm /*+ m_resolutionMm/2*/) / m_resolutionMm;
    if( posIndex >= (qint32)m_nbCell || posIndex < 0 ) {
        posIndex = -1;
    }
    return posIndex;
}

qreal Surface1d::indexToPosition(qint32 p_index)
{
    qreal pos = m_surfaceStartMm + p_index * m_resolutionMm /*+ m_resolutionMm / 2*/;
    return pos;
}

//void Surface1d::setMeasureToDisplay(qint32 p_value)
//{
//    if( m_measureType != p_value ) {
//        // update measure :
//        m_measureType = p_value;
//        for(quint32 i=0; i<m_nbCell; i++) {
//            SurfaceCell * pCell = m_aCell[i];
//            if( nullptr != pCell->getAScanReference() ) {
//                if( MEASURE_ME == m_measureType ) {
//                    m_aMeasure[i] = pCell->getAScanReference()->getAScanMeasure()->m_thicknessMmMultiEcho;
//                } else {
//                    m_aMeasure[i] = pCell->getAScanReference()->getAScanMeasure()->m_thicknessMm1Echo;
//                }
//            }
//        }
//        emit evSurfaceChanged();
//    }
//}

//qint32 Surface1d::getMeasureToDisplay()
//{
//    return m_measureType;
//}

//void Surface1d::setResolution(qreal p_value);
qreal Surface1d::getResolution()
{
    return m_resolutionMm;
}

void Surface1d::setSurfaceStartMm(qreal p_value)
{
    m_surfaceStartMm = p_value;
}

qreal Surface1d::getSurfaceStartMm() const
{
    return m_surfaceStartMm;
}


quint32 Surface1d::getNbCell(void) const
{
    return m_nbCell;
}


qreal Surface1d::getSurfaceEnd(void) const
{
    return m_surfaceStartMm+(m_nbCell-1)*m_resolutionMm;
}

QVector<SurfaceCell *> *Surface1d::getCell()
{
    return &m_aCell;
}

QVector<qreal> * Surface1d::getArrayPositionM(void)
{
    return &m_aPositionM;
}

QVector<qreal> * Surface1d::getArrayMeasure1e(void)
{
    return &m_aMeasure1e;
}

QVector<qreal> * Surface1d::getArrayMeasureMe(void)
{
    return &m_aMeasureMe;
}

qreal Surface1d::getPositionMin() const
{
    return m_positionMin;
}

qreal Surface1d::getPositionMax() const
{
    return m_positionMax;
}

qreal Surface1d::getMeasureMin() const
{
    return m_measureMin;
}

qreal Surface1d::getMeasureMax() const
{
    return m_measureMax;
}

void Surface1d::lockData()
{
    m_mutData.lock();
}

void Surface1d::unlockData()
{
    m_mutData.unlock();
}

