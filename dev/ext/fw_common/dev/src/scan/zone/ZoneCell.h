/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ZoneCell.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class LocationMeasure;

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class ZoneCell

/**
 * @brief the ZoneCell class
 *
 */
class ZoneCell : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit ZoneCell();
    virtual ~ZoneCell();

    virtual void computeValueFromMeasures(void);

    void addMeasure(LocationMeasure * p_measure);

    qreal getValue(void) const;
    void setValue(qreal p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

signals :
    void evValueChanged();

protected :

    // -- Properties
    qreal m_value;  ///< value computed from m_lLocationMeasure
    QList<LocationMeasure *> m_lLocationMeasure;
};


