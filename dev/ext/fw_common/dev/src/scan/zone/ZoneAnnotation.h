/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ZoneAnnotation.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>

// -- Qt includes
#include <QColor>
#include <QPointF>


/**
 * @brief the ZoneAnnotation class
 *
 */
class ZoneAnnotation : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit ZoneAnnotation(const QString &p_annotationName);
    virtual ~ZoneAnnotation();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Properties
    QPointF getPosition(void) const;
    void setPosition(const QPoint& p_value);
    QColor getColor(void) const;
    void setColor(const QColor & p_value);

protected :

    // -- Properties
    QPointF m_position; ///< position in the zone
    QColor m_color;     ///< foreground color

};

