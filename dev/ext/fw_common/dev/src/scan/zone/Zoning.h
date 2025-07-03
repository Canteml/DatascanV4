/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XXX.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class Zone;
class ZoningBacIntoMepZone;
class Location;

// -- Qt includes
#include <QList>

// ----------------------------------------------------------------------------
// -- Class Zoning

/**
 * @brief the Zoning class
 *
 */
class Zoning : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    enum ZoningType {
        TYPE_UNDEF,
        TYPE_UNIQUE,         ///< only one zone for all the piece
        TYPE_FREE,           ///< free zone, like a patchwork
        TYPE_MAT_MEP,        ///< matrice for MEP
        TYPE_MAT_VERTICAL,   ///< vertical lines of zones ("generatrices")
        TYPE_MAT_HORIZONTAL  ///< horizontal lines of zones ("circulaires")
    };

    enum ZoningOperatingPath {
        OPERATING_MANUAL,               ///< "no path"
        OPERATING_VERTICAL_CW_ZZDU,     ///< ClockWise ZigZag Down to Up
        OPERATING_VERTICAL_CW_ZZUD,     ///< ClockWise ZigZag Up to Down
        OPERATING_VERTICAL_CW_DU,       ///< ClockWise Down to Up
        OPERATING_VERTICAL_CW_UD,       ///< ClockWise ZigZag Down to Up
        OPERATING_VERTICAL_CCW_ZZDU,
        OPERATING_VERTICAL_CCW_ZZUD,
        OPERATING_VERTICAL_CCW_DU,
        OPERATING_VERTICAL_CCW_UD,
        OPERATING_HORIZONTAL_CW_ZZDU,
        OPERATING_HORIZONTAL_CW_ZZUD,
        OPERATING_HORIZONTAL_CW_DU,
        OPERATING_HORIZONTAL_CW_UD,
        OPERATING_HORIZONTAL_CCW_ZZDU,
        OPERATING_HORIZONTAL_CCW_ZZUD,
        OPERATING_HORIZONTAL_CCW_DU,
        OPERATING_HORIZONTAL_CCW_UD
    };

    explicit Zoning(quint32 p_type);
    explicit Zoning(const Zoning & p_zoning);
    virtual ~Zoning();


    // -- Behaviour
    quint32 getZoningId(void) const;
    void setZoningId(quint32 p_value);
    quint32 getZoningType() const;
    void setZoningType(quint32 p_value);

    // -- Data
    void setZoneMatrixSize(quint32 p_x, quint32 p_y);
    qint32 getZoneMatrixSizeX() const;
    qint32 getZoneMatrixSizeY() const;
    Zone * getZoneItem(int p_x, int p_y) const;
    void removeAllZone();
    void addRow(int p_index);
    void addColumn(int p_index);
    void removeRow(int p_index);
    void removeColumn(int p_index);

    quint32 getLocationListSize() const;
    Location * getLocation(int p_index) const;
    void addLocation(int p_index, Location * p_pLocation);
    void removeLocation(int p_index);
    void removeAllLocation();

    // -- Convenient methods
    bool isOperatingPathCW() const;
    bool isOperatingPathCCW() const;
    bool isOperatingVertical() const;
    bool isOperatingHorizontal() const;
    bool isOperatingManual() const;

    // -- Parameters
    QString getZoningName(void) const;
    void setZoningName(const QString & p_value);
    quint32 getZoningOperatingPath(void) const;     ///< value in Zoning::ZoningOperatingPath
    void setZoningOperatingPath(quint32 p_value);    ///< value in Zoning::ZoningOperatingPath
    qreal getZoningCellSpacingCm(void) const;
    void setZoningCellSpacingCm(qreal p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Operators
    bool operator==(const Zoning & p_value) const;
    Zoning &operator=(const Zoning & p_value);
    Zoning *getCopy() const;

signals :
    void evZoningOperatingPathChanged();

protected :
    // -- Behaviour
    explicit Zoning(quint32 p_type, const QString& p_zoningName);  ///< protected constructor for inherited classes
    void deleteMat();

    quint32 m_zoningId;
    quint32 m_zoningType;

    // -- Data
    QVector<QVector<Zone *>> m_matZone;
    QList<Location *> m_lLocation;

    // -- Parameters
    QString m_zoningName;
    quint32 m_zoningOperatingPath;
};
