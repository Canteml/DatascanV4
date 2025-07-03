#include "DisplaySurface.h"

#include <v2/scan/Location1d.h>
#include <v2/scan/AScan.h>
#include <v2/scan/SurfaceCell.h>

DisplaySurface::DisplaySurface(Location1d* p_pLocation1d)
    : Surface1d(p_pLocation1d)
{
    setLogName("DisplaySurface");
}


qint32 DisplaySurface::pushAScan(AScan* p_pAScan)
{
    qreal pos = p_pAScan->getAScanRawSample()->m_position.x();
    qint32 posIndex = ( pos - m_surfaceStartMm + m_resolutionMm/2) / m_resolutionMm;
    qint32 posIndexLeft = posIndex - 1; //( pos - m_surfaceStartMm - m_resolutionMm/2 ) / m_resolutionMm;
    qint32 posIndexRight = posIndex + 1;
    //fLogDebug1(QString("pos = %1 / index = %2 / start = %3 ").arg(pos).arg(posIndex).arg(m_surfaceStartMm));

    // -- Adjust size of arrays
    // first ASCAN ?

    lockData();
    bool bFirst = false;
    if( 0 == m_nbCell ) {
        m_surfaceStartMm = m_resolutionMm*((qint32)(pos)/((qint32)m_resolutionMm)) - m_resolutionMm;
        for(int i = 0 ; i < 3 ; i++) {
            m_aCell.append(new SurfaceCell(m_pLocation1d));
            m_aMeasure1e.append(0);
            m_aMeasureMe.append(0);
        }
        m_aPositionM.append(m_surfaceStartMm/1000);
        m_aPositionM.append((m_surfaceStartMm + m_resolutionMm)/1000);
        m_aPositionM.append((m_surfaceStartMm + 2*m_resolutionMm)/1000);
        m_nbCell = 3;
        bFirst = true;
    } else {
        // resize arrays if needed :
        if( posIndexRight >= (qint32)m_nbCell) {
            // add cells at the end
            qint32 cellToAppend = 1+posIndexRight-m_nbCell;
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
        if( posIndexLeft<0 ) {
            // insert cells at begin
            qint32 cellToInsert = -posIndexLeft;
            for(qint32 i=0; i <= cellToInsert; i++) {
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
        }
    }

    /// -- Set cell with new ASCAN
    posIndex = ( pos - m_surfaceStartMm /*+ m_resolutionMm/2*/) / m_resolutionMm; // [EDIT]
    posIndexLeft = posIndex - 1;
    posIndexRight = posIndex + 1;
    // find Cell :
    posIndexLeft = saveToCell(posIndexLeft, p_pAScan, bFirst);
    posIndex = saveToCell(posIndex, p_pAScan, bFirst);
    posIndexRight = saveToCell(posIndexRight, p_pAScan, bFirst);

    unlockData();

    if( (-1 != posIndex) || (-1 != posIndexLeft) || (-1 != posIndexRight) ) {
        emit evSurfaceChanged();
    }

    return posIndex;
}
