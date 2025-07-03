/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckZoning.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>
class Zoning;
class CheckLocation;
class CheckZone;

// -- Qt includes
#include <QVector>

// ----------------------------------------------------------------------------
// -- Class CheckZoning

/**
 * @brief the CheckZoning class
 *
 * - set samples in the measure of the current selected location
 * - catch new measure value of location and set them into zone
 *
 *
 */
class CheckZoning : public QObject, public XmlProjectLoggable
{
public :
    explicit CheckZoning();
    virtual ~CheckZoning();

    // -- Data


    void setZoneMatrixSize(quint32 p_x, quint32 p_y);
    quint32 getZoneMatrixSizeX() const;
    quint32 getZoneMatrixSizeY() const;
    CheckZone * getZoneItem(int p_x, int p_y) const;
    void removeAllZone();

    quint32 getLocationListSize() const;
    CheckLocation * getLocation(int p_index) const;
    void addLocation(int p_index, CheckLocation * p_pLocation);
    void removeLocation(int p_index);
    void removeAllLocation();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    Zoning * m_pZoningReference;
    QList<CheckLocation *> m_lCheckLocation;
    QVector<QVector<CheckZone *>> m_matCheckZone;

};


