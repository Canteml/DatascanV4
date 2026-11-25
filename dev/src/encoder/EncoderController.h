/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : EncoderController.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QVariantList>

class UtPoleEncoderReader;
class VectorStd;
class ComLinkManager;

// -- EncoderController

class EncoderController : public QObject
{
    Q_OBJECT

public:
    explicit EncoderController(QObject* parent = nullptr);

    bool activate(VectorStd* vectorStd, const QString& portName = "COM4");
    void deactivate();

    bool isActive() const { return m_active; }

    void setLinkManager(ComLinkManager* linkManager);
    void setPositionVector(VectorStd* vectorStd);
    Q_INVOKABLE QVariantList availableDevices() const;

    Q_PROPERTY(QString bscanSource
               READ  bscanSource
               WRITE setBScanSource
               NOTIFY bscanSourceChanged)

    QString bscanSource() const { return m_bscanSource; }
    void    setBScanSource(const QString& value);

    Q_PROPERTY(qint32 baudRate
               READ  baudRate
               WRITE setBaudRate
               NOTIFY baudRateChanged)

    qint32 baudRate() const { return m_baudRate; }
    void   setBaudRate(qint32 value);

    Q_PROPERTY(qreal calibrationRealMm
               READ  calibrationRealMm
               WRITE setCalibrationRealMm
               NOTIFY calibrationRealMmChanged)
    Q_PROPERTY(qreal calibrationEstimatedMm
               READ  calibrationEstimatedMm
               WRITE setCalibrationEstimatedMm
               NOTIFY calibrationEstimatedMmChanged)
    Q_PROPERTY(qreal wheelDiameterMm READ wheelDiameterMm WRITE setWheelDiameterMm NOTIFY wheelDiameterMmChanged)
    Q_PROPERTY(qint32 coderResolution READ coderResolution WRITE setCoderResolution NOTIFY coderResolutionChanged)
    Q_PROPERTY(bool encoderReverse READ encoderReverse WRITE setEncoderReverse NOTIFY encoderReverseChanged)

    // -- Calibration

    Q_INVOKABLE void setCalibration(qreal zeroMm, qreal realMm, qreal estimatedMm);
    Q_INVOKABLE void calibrateNow(qreal realMm, qreal estimatedMm);

    qreal zeroMm()        const { return m_zeroMm; }
    qreal scaleFactor()   const { return m_scaleFactor; }

    qreal calibrationRealMm()      const { return m_calibrationRealMm; }
    qreal calibrationEstimatedMm() const { return m_calibrationEstimatedMm; }

    void setCalibrationRealMm(qreal value) {
        if( m_calibrationRealMm != value ) {
            m_calibrationRealMm = value;
            emit calibrationRealMmChanged(value);
        }
    }
    void setCalibrationEstimatedMm(qreal value) {
        if( m_calibrationEstimatedMm != value ) {
            m_calibrationEstimatedMm = value;
            emit calibrationEstimatedMmChanged(value);
        }
    }

    qreal   wheelDiameterMm() const { return m_wheelDiameterMm; }
    void    setWheelDiameterMm(qreal value);
    qint32  coderResolution() const { return m_coderResolution; }
    void    setCoderResolution(qint32 value);
    bool    encoderReverse() const { return m_encoderReverse; }
    void    setEncoderReverse(bool value);

signals:
    void evError(const QString& msg);
    void evActivated();
    void evDeactivated();
    void availableDevicesChanged();
    void baudRateChanged(qint32);
    void bscanSourceChanged(const QString&);
    void calibrationRealMmChanged(qreal);
    void calibrationEstimatedMmChanged(qreal);
    void wheelDiameterMmChanged(qreal);
    void coderResolutionChanged(qint32);
    void encoderReverseChanged(bool reverse);
    void odometryChanged(qreal wheelDiameterMm, qint32 coderResolution);


private slots:
    void onPositionMm(qreal mm);
    void onReaderError(const QString& msg);

    void onProdIdChanged(quint32 prodId);

private:
    qreal calibratedPosition(qreal rawMm) const;

    UtPoleEncoderReader* m_samd21Reader   = nullptr;
    VectorStd*           m_vectorStd      = nullptr;
    ComLinkManager*      m_linkManager    = nullptr;
    bool           m_active        = false;
    QString        m_portName;
    qint32         m_baudRate      = 115200;
    VectorStd*     m_positionVector = nullptr;
    QString        m_bscanSource;
    quint32        m_positionCount = 0;
    qreal          m_lastRawMm     = 0.0;

    // Calibration state
    qreal  m_zeroMm                 = 0.0;
    qreal  m_scaleFactor            = 1.0;
    qreal  m_calibrationRealMm      = 100.0;
    qreal  m_calibrationEstimatedMm = 100.0;

    bool    isUtPoleProfile() const;
    QString odometryKey(const char* name) const;
    qreal   utPolePulsesPerMm() const;
    void    loadOdometry();
    void    applyOdometry();

    // Odometry
    qreal   m_wheelDiameterMm = 56.8;
    qint32  m_coderResolution = 82084;
    bool    m_encoderReverse  = false;
};
