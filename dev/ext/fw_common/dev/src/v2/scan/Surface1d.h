/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Surface1d.h
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
class AScan;
class Location1d;
class SurfaceCell;

// -- Qt Includes
#include <QObject>

/**
 * @brief The Surface1d class
 *
 * Data are stored in 1d array of cells. Index of array give the position :
 *
 * index   :  |     0        |      1          |      2          | . | . | . | indexMax
 * pos.mm  :  | surfaceStart | surfaceStart +  | surfaceStart +  | . | . | . | surfaceEnd
 *                                resMm * 1         resMm * 2
 */
class Surface1d : public QObject, public Loggable {
    Q_OBJECT
public :
    Surface1d(Location1d* p_pLocation1d);
    ~Surface1d();

    void clearData();
    qint32 pushAScan(AScan* p_pAScan);
    qint32 saveToCell(qint32 p_posIndex, AScan* p_pAScan, bool p_bFirst = false);

    void setMeasureToDisplay(qint32 p_value);   ///< value in MEASURE_TYPE
    qint32 getMeasureToDisplay();

//    void setResolution(qreal p_value); <== constant for now
    qreal getResolution();

    void setSurfaceStartMm(qreal p_value);
    qreal getSurfaceStartMm() const;

    quint32 getNbCell(void) const;

    qreal getSurfaceEnd(void) const;

    QVector<SurfaceCell*> * getCell(void);
    QVector<qreal> * getArrayPositionM(void);
    QVector<qreal> * getArrayMeasure1e(void);
    QVector<qreal> * getArrayMeasureMe(void);

    qreal getPositionMin() const;
    qreal getPositionMax() const;

    qreal getMeasureMin() const;
    qreal getMeasureMax() const;

    void lockData();
    void unlockData();

    qint32 positionToIndex(qreal p_posMm); ///< return -1 if position is invalid
    qreal indexToPosition(qint32 p_index); ///<

signals :
    void evSurfaceChanged();    ///< request a visual update
    void evZoneChanged();

protected :
    // Behaviour
    Location1d* m_pLocation1d;  ///< parent, owner of AScan

    static const qreal DEFAULT_CELL_RESOLUTION_MM;
    qreal m_resolutionMm;       ///< resolution of the surface : width of a one cell
    qreal m_surfaceStartMm;
    quint32 m_nbCell;       

    qreal m_positionMin;
    qreal m_positionMax;
    qreal m_measureMin;
    qreal m_measureMax;

    // following array has
    QVector<SurfaceCell*> m_aCell;  ///< references to cell that store ASCAN
    QVector<qreal> m_aPositionM;     ///< positions of cells and measure to display (Axe X)
    QVector<qreal> m_aMeasureMe;      ///< measures Multi-Echo to display (BSCAN)
    QVector<qreal> m_aMeasure1e;      ///< measures 1E-Echo to display (BSCAN)

    QMutex m_mutData;

};
