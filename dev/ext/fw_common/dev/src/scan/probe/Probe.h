/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Probe.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>
#include <scan/type/Calibrate.h>
class MeasureParam;
class Coupling;

// -- Qt includes
#include <QSizeF>


// ----------------------------------------------------------------------------
// -- Class ProbeParam

/**
 * @brief the ProbeParam class
 *
 */
class ProbeParam : public QObject, public XmlProjectLoggable
{
public :
    explicit ProbeParam();
    explicit ProbeParam(const ProbeParam & p_value);   ///< copy constructor
    virtual ~ProbeParam();

    virtual quint32 getProbeType(void) const = 0;       ///< value in Probe::ProbeType
    virtual ProbeParam* copy() const = 0;

    quint32 getId(void) const;
    void setId(quint32 p_value);

    QSizeF getParamSensorSize() const;
    void setParamSensorSize(const QSizeF & p_value);
    QString getParamSensorReference(void) const;
    void setParamSensorReference(const QString & p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    ProbeParam & operator= (const ProbeParam & p_value);
    bool operator== (const ProbeParam & p_value);

protected :

    quint32 m_ID;       ///< used as reference if data base are available (0 = invalid ID)

    // -- Parameters
    QSizeF m_paramSensorSize;
    QString m_paramSensorReference;
};

// ----------------------------------------------------------------------------
// -- Class ProbeCalibrate

// \todo : develop MeasureParam and Coupling

class ProbeCalibrate : public Calibrate
{
public :

    enum ProbeCalibrateType {
        TYPE_UNDEF,
        TYPE_USKEY_STEP_WEDGE
    };

    explicit ProbeCalibrate();
    explicit ProbeCalibrate(const ProbeCalibrate & p_value);    ///< copy constructor
    virtual ~ProbeCalibrate();

    virtual quint32 getProbeCalibrateType(void) const = 0;      ///< value in ProbeCalibrate::Type
    virtual ProbeCalibrate* copy() const = 0;

    // -- operator
    ProbeCalibrate & operator= (const ProbeCalibrate & p_value);
    bool operator== (const ProbeCalibrate & p_value);

protected :
    //MeasureParam * m_pMeasurParam;
    //Coupling * m_pCoupling;
};

// ----------------------------------------------------------------------------
// -- Class Probe

/**
 * @brief the Probe class
 *
 * A probe is a couple of one sensor and its digitization interface
 *
 * A Probe has :
 * - parameters : references (sensor / electronic ...)
 * - a set of params to configure the probe
 * - a set of params to configure the measure calculation (depend of the type of calculation method)
 *
 *  The probe type value is in m_pProbeParam
 */
class Probe : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    enum ProbeType {
        TYPE_UNDEF,
        TYPE_USKEY
    };

    static QString PobeTypeName(quint32 e);       ///< return a string with the name of the probe type

    explicit Probe(ProbeParam * p_pProbeParam, ProbeCalibrate * p_pProbeCalibrate);
    virtual ~Probe();

    quint32 getProbeType(void) const;   ///< return a value in Probe::ProbeType
    QString getProbeTypeName(void) const;   ///< return the name of the probe

    void setProbeParam(const ProbeParam & p_value);
    ProbeParam * getProbeParamCopy(void) const;
    ProbeParam * getProbeParam(void) const;

    ProbeCalibrate * getProbeCalibrate() const;
    void setProbeCalibrate(const ProbeCalibrate &p_value);

    // -- Parameters
    QString getParamSensorBrand(void) const;
    void setParamSensorBrand(const QString & p_value);
    QString getParamSensorType(void);
    void setParamSensorType(const QString & p_value);
    QString getParamSensorSerialNumber(void);
    void setParamSensorSerialNumber(const QString & p_value);
    QString getParamSensorWave(void);
    void setParamSensorWave(const QString & p_value);
    QString getParamSensorFrequency(void);
    void setParamSensorFrequency(const QString & p_value);
    QString getParamSensorAngle(void);
    void setParamSensorAngle(const QString & p_value);
    QString getParamSensorPiezoSize(void);
    void setParamSensorPiezoSize(const QString & p_value);
    quint32 getParamSensorNumberElements(void);
    void setParamSensorNumberElements(quint32 p_value);


    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    static Probe *makeProbe(quint32 p_type);

protected :

    // -- Parameters (Sensor references)
    QString m_paramSensorBrand; ///< (Marque)
    QString m_paramSensorType;
    QString m_paramSensorSerialNumber;
    QString m_paramSensorWave;
    QString m_paramSensorFrequency;
    QString m_paramSensorAngle;
    QString m_paramSensorPiezoSize;
    quint32 m_paramSensorNumberElements;    ///< one sensor is usually made with 1 cell to generate the pulse, and one cell to measure the echos

    // -- Composition
    ProbeParam * m_pProbeParam;
    ProbeCalibrate * m_pProbeCalibrate;
};


