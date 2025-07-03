/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ProbeUsKey.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/probe/Probe.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class ProbeParamUsKey

class ProbeParamUsKey : public ProbeParam
{
public :
    explicit ProbeParamUsKey();
    explicit ProbeParamUsKey(const ProbeParamUsKey & p_value);   ///< copy constructor
    virtual ~ProbeParamUsKey();

    // -- ProbeParam
    virtual quint32 getProbeType(void) const override;       ///< value in Probe::ProbeType
    virtual ProbeParam* copy() const override;

    // -- Parameters
    double getParamFilter() const;
    void setParamFilter(double p_paramFilter);
    double getParamSensorMode() const;
    void setParamSensorMode(double p_paramSensorMode);
    double getParamGain() const;
    void setParamGain(double p_paramGain);
    double getParamVoltage() const;
    void setParamVoltage(double p_paramVoltage);
    double getParamWidth() const;
    void setParamWidth(double p_paramWidth);
    double getParamSamplingFreqMode() const;
    void setParamSamplingFreqMode(double p_paramSamplingFreqMode);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // implicit copy assignment operator should do the job with these attributs

private :
    double m_paramFilter;               ///< 0 = 1.25MHz, 2 = 2.5 MHz, 3 = 10 MHs, 4 = Broad band
    double m_paramSensorMode;           ///< 0 = mono element / 1 = bi-element
    double m_paramGain;                 ///< (dB)
    double m_paramVoltage;              ///< 10 <= val <= 230
    double m_paramWidth;                ///< 0 <= val <= 255
    double m_paramSamplingFreqMode;     ///< 0=160 1=80 2=40 3=20
};

// ----------------------------------------------------------------------------
// -- Class ProbeCalibrateUsKey

class ProbeCalibrateUsKey : public ProbeCalibrate
{
public :
    explicit ProbeCalibrateUsKey();
    explicit ProbeCalibrateUsKey(const ProbeCalibrateUsKey & p_value);
    ~ProbeCalibrateUsKey();

    // -- ProbeParam
    virtual quint32 getProbeCalibrateType(void) const override;       ///< value in ProbeCalibrate::ProbeCalibrateType
    virtual ProbeCalibrate* copy() const override;

    // -- Parameters
    QString getParamWedgeType(void) const;
    void setParamWedgeType(const QString & p_value);
    QString getParamWedgeThicknesses(void) const;
    void setParamWedgeThicknesses(const QString & p_value);
    QString getParamCalibGainRef(void) const;
    void setParamCalibGainRef(const QString & p_value);
    QString getParamCalibGainControl(void) const;
    void setParamCalibGainControl(const QString & p_value);
    QString getParamCalibBaseTime(void) const;
    void setParamCalibBaseTime(const QString & p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // implicit copy assignment operator should do the job with these attributs

private :
    // -- Wedge (cale)
    QString m_paramWedgeType;           // usually Step Wedge
    QString m_paramWedgeThicknesses;
    QString m_paramCalibGainRef;
    QString m_paramCalibGainControl;
    QString m_paramCalibBaseTime;
};

// ----------------------------------------------------------------------------
// -- Class ProbeUsKey

/**
 * @brief the ProbeUsKey class
 *
 * ProbeUsKey has 2 kinds of data :
 * - References : only used for reports
 * - Properties : used to dynamically configure the UsKey
 *
 */
class ProbeUsKey : public Probe
{
    Q_OBJECT

public :
    ProbeUsKey();
    ~ProbeUsKey();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- References
    QString getSensorBrand() const;
    void setParamSensorBrand(const QString & p_sensorBrand);
    QString getSensorType() const;
    void setSensorType(const QString & p_sensorType);
    QString getSensorSerialNumber() const;
    void setSensorSerialNumber(const QString & p_sensorSerialNumber);
    QString getSensorWave() const;
    void setSensorWave(const QString & p_sensorWave);
    QString getSensorAngle() const;
    void setSensorAngle(const QString & p_sensorAngle);
    QString getSensorFrequency() const;
    void setSensorFrequency(const QString & p_sensorFrequency);
    quint32 getSensorNbElements() const;
    void setSensorNbElements(const quint32 p_sensorNbElements);
    qreal getSensorPiezoSizeMm() const;
    void setSensorPiezoSizeMm(qreal p_sensorPiezoSizeMm);

private :

    // -- static References
    QString m_sensorBrand;
    QString m_sensorType;           ///< Brand part
    QString m_sensorSerialNumber;
    QString m_sensorWave;
    QString m_sensorAngle;
    QString m_sensorFrequency;
    quint32 m_sensorNbElements;
    qreal m_sensorPiezoSizeMm;

    // -- Parameters (probe properties)
    ProbeParamUsKey m_probeParamUsKey;
    ProbeCalibrateUsKey m_probeCalibrateUsKey;
};

