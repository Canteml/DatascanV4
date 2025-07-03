/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Zone.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
#include <scan/zone/ZoneAnnotation.h>
#include <scan/type/SystemTransformation.h>

// -- Qt includes
#include <QPointF>
#include <QSizeF>

// ----------------------------------------------------------------------------
// -- Class Zone

/**
 * @brief the Zone class
 *
 */
class Zone : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    // -- Type
    enum ZoneType {
        TYPE_UNDEF,
        TYPE_INEXISTENT,
        TYPE_INACCESSIBLE,
        TYPE_ACCESSIBLE
    };

    Zone();
    ~Zone();

    Zone *copy();

    // -- Behaviour
    quint32 getZoneType(void) const;
    void setZoneType(quint32 p_value);

    // -- Parameters
    QString getZoneTitle(void) const;
    void setZoneTitle(const QString& p_value);
    SystemTransformation getZoneTransformation(void) const;
    void setZoneTransfromation(const SystemTransformation& p_value);
    QSizeF getZoneSize(void) const;
    void setZoneSize(const QSizeF& p_value);
    QSizeF getZoneResolution(void) const;
    void setZoneResolution(const QSizeF& p_value);
    QString getZoneXAxisTitle(void) const;
    void setZoneXAxisTitle(const QString& p_value);
    QString getZoneYAxisTitle(void) const;
    void setZoneYAxisTitle(const QString& p_value);
    qreal getZoneOriginalThickness(void) const;
    void setZoneOriginalThickness(qreal p_value);

    // -- Composition
    // \todo

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    // -- Behaviour
    quint32 m_zoneType; ///< value in ZoneType

    // -- Parameters
    QString m_zoneTitle;
    SystemTransformation m_zoneTransformation;  ///< translation and rotation from piece system (parent)
    QSizeF m_zoneSize;
    QSizeF m_zoneResolution;
    QString m_zoneXAxisTitle;
    QString m_zoneYAxisTitle;
    qreal m_zoneOriginalThickness;

    // -- Composition : \todo
    //QList<ZoneAnnotation *> m_lZoneAnnotation;
    //EquipmentPreset

};

