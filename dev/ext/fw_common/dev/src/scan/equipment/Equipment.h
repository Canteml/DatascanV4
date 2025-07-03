/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Equipment.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>

class Coupling;
class Vector;
class Probe;

// -- Qt includes
#include <QPointF>
#include <QSizeF>

// ----------------------------------------------------------------------------
// -- Class Equipment

/**
 * @brief the Equipment class
 *
 */
class Equipment : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    /**
     * @brief The EquipmentTypeStd enum
     *
     * Standard types of equipment : vector + probe
     */
    enum EquipmentTypeStd {
        TYPE_UNDEF,
        TYPE_UTPOLE_NOT_STD,
        TYPE_UTPOLE_USKEY,
        TYPE_ALTISCAN_USKEY
    };

    static QString EquipmentTypeName(quint32 e);        ///< return a string with the name of the probe type

    explicit Equipment(Vector* p_pVector, Probe* m_pProbe, Coupling* m_pCoupling);
    explicit Equipment();
    ~Equipment();    

    virtual quint32 getEquipmentTypeStd(void) const;    ///< return a value in Equipment::EquipmentType
    QString getProbeTypeStdName(void) const;            ///< return the name of the Equipment

    // -- Properties
    QPointF getResolutionMax() const;
    qreal getFrequencyAcqMax() const;
    QSizeF getZoneScanningMax() const;
    qreal getSpeedMaxMMS() const;

    // -- Composition accessors

    void setVector(Vector * p_vector);
    Vector * getVector() const;

    void setPRobe(Probe * p_probe);
    Probe * getProbe() const;

    void setCoupling(Coupling * p_coupling);
    void setCoupling(const Coupling & p_coupling);
    Coupling * getCoupling() const;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    static Equipment * makeNewEquipmentStd(quint32 p_type);    ///< p_type in EquipmentTypeStd
    static Equipment * makeNewEquipment(quint32 p_typeVector, quint32 p_typeProbe);

protected :

    // -- Behaviour
    bool computeProperties();

    // ---- Attributs

    // -- References
    quint32 m_type;

    // -- Computed properties
    QPointF m_resolutionMax;
    qreal m_frequencyAcqMax;
    QSizeF m_zoneScanningMax;  ///< take care of the probe geometry and the vector sweeping zone.
    qreal m_speedMaxMMS;        ///< max speed, in millimeters/second

    // -- Composition (children)
    Vector * m_pVector;
    Probe * m_pProbe;
    Coupling * m_pCoupling;

};

