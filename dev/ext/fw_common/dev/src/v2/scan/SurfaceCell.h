/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : SurfaceCell.h
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
class AScan;
class Location1d;

// -- Qt Includes
#include <QObject>

/**
 * @brief The SurfaceCell class
 */

class SurfaceCell : public QObject, public Loggable {
    Q_OBJECT
public :
    SurfaceCell(Location1d* p_pLocation1d);
    ~SurfaceCell();

    bool pushAScan(AScan* p_pAScan);

    AScan* getAScanReference() const;
    QString getTemperatureStr() const;
    QString getHumidityStr() const;

    void lockData();
    void unlockData();


signals :
    void evCellChanged(SurfaceCell*);


private :
    QList<AScan*> m_lAScan;     ///< list of AScan
    Location1d* m_pLocation1d;  ///< owner of AScan
    AScan* m_pAScanReference;   ///< best ascan that represent this cell

    qreal m_rTemperature;
    qreal m_rHumidity;

    QMutex m_mutData;

};
