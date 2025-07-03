/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckLocation.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class LocationMeasure;

// -- Qt includes


/**
 * @brief the CheckPiece class
 *
 */
class CheckLocation : public QObject, public XmlProjectLoggable
{
public :
    explicit CheckLocation();
    virtual ~CheckLocation();

    void clearMeasure();
    void addMeasure(LocationMeasure * m_pMeasure);
    void makeMeasure(QPointF p_position, QVector<qint16> *p_samples);
    void makeMeasure(QPointF p_position, QVector<qint16> *p_samples, qreal p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    // -- Properties
    quint32 m_LocationReferenceId;
    QList<LocationMeasure *> m_lMeasure;


};


