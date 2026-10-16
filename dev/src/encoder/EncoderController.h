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

    // Network devices (authenticated links) are listed first, then serial ports.
    // Each entry: { "text": label shown, "address": "192.168.0.50:2110" or "COM4" }
    void setLinkManager(ComLinkManager* linkManager);
    Q_INVOKABLE QVariantList availableDevices() const;

    Q_PROPERTY(qreal calibrationRealMm
               READ  calibrationRealMm
               WRITE setCalibrationRealMm
               NOTIFY calibrationRealMmChanged)
    Q_PROPERTY(qreal calibrationEstimatedMm
               READ  calibrationEstimatedMm
               WRITE setCalibrationEstimatedMm
               NOTIFY calibrationEstimatedMmChanged)

    // -- Calibration

    Q_INVOKABLE void setCalibration(qreal zeroMm, qreal realMm, qreal estimatedMm);
    Q_INVOKABLE void calibrateNow(qreal realMm, qreal estimatedMm);
    Q_INVOKABLE void resetCalibration();

    bool  isCalibrated()  const { return m_calibrated; }
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

signals:
    void evError(const QString& msg);
    void evActivated();
    void evDeactivated();
    void availableDevicesChanged();
    void calibrationRealMmChanged(qreal);
    void calibrationEstimatedMmChanged(qreal);

private slots:
    void onPositionMm(qreal mm);
    void onReaderError(const QString& msg);

private:
    qreal calibratedPosition(qreal rawMm) const;

    UtPoleEncoderReader* m_samd21Reader   = nullptr;
    VectorStd*           m_vectorStd      = nullptr;
    ComLinkManager*      m_linkManager    = nullptr;
    bool           m_active        = false;
    quint32        m_positionCount = 0;
    qreal          m_lastRawMm     = 0.0;

    // Calibration state
    bool   m_calibrated             = false;
    qreal  m_zeroMm                 = 0.0;
    qreal  m_scaleFactor            = 1.0;
    qreal  m_calibrationRealMm      = 100.0;
    qreal  m_calibrationEstimatedMm = 100.0;
};
