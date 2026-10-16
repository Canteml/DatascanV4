/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : EncoderController.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <encoder/EncoderController.h>
#include <encoder/UtPoleEncoderReader.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/com/ComLinkManager.h>
#include <log/Logger.h>

#include <QSerialPortInfo>
#include <QVariantMap>
#include <algorithm>

// ----------------------------------------------------------------------------

EncoderController::EncoderController(QObject* parent)
    : QObject(parent)
{
}

void EncoderController::setLinkManager(ComLinkManager* linkManager)
{
    if (m_linkManager)
        disconnect(m_linkManager, nullptr, this, nullptr);

    m_linkManager = linkManager;
    if (m_linkManager) {
        connect(m_linkManager, &ComLinkManager::evNewLinkAuthentified,
                this, &EncoderController::availableDevicesChanged);
        connect(m_linkManager, &ComLinkManager::evLinkDisconnected,
                this, &EncoderController::availableDevicesChanged);
    }
    emit availableDevicesChanged();
}

QVariantList EncoderController::availableDevices() const
{
    static constexpr quint16 VID_INTEL = 0x8086;

    QVariantList list;

    if (m_linkManager) {
        const QStringList linkNames = m_linkManager->getAuthentifiedLinkNames();
        for (const QString& name : linkNames) {
            QVariantMap entry;
            entry["address"] = name;
            entry["text"]    = name;
            list.append(entry);
        }
    }

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    std::sort(ports.begin(), ports.end(),
              [](const QSerialPortInfo& a, const QSerialPortInfo& b) {
                  if (a.hasVendorIdentifier() != b.hasVendorIdentifier())
                      return a.hasVendorIdentifier();
                  return a.portName().mid(3).toInt() < b.portName().mid(3).toInt();
              });

    for (const QSerialPortInfo& portInfo : ports) {
        if (portInfo.hasVendorIdentifier() && portInfo.vendorIdentifier() == VID_INTEL)
            continue;

        QVariantMap entry;
        entry["address"] = portInfo.portName();
        entry["text"]     = portInfo.description().isEmpty()
                            ? portInfo.portName()
                            : QString("%1 - %2").arg(portInfo.portName(), portInfo.description());
        list.append(entry);
    }
    return list;
}

bool EncoderController::activate(VectorStd* vectorStd, const QString& portName)
{
    if (m_active)
        return true;

    m_vectorStd = vectorStd;

    m_samd21Reader = new UtPoleEncoderReader(this);
    connect(m_samd21Reader, &UtPoleEncoderReader::evPositionMm, this, &EncoderController::onPositionMm);
    connect(m_samd21Reader, &UtPoleEncoderReader::evError,      this, &EncoderController::onReaderError);
    if (!m_samd21Reader->open(portName)) {
        delete m_samd21Reader;
        m_samd21Reader = nullptr;
        return false;
    }
    Logger::logInfo(QString("[EncoderController] UT POLE reader active on %1").arg(portName));

    m_active = true;
    emit evActivated();
    return true;
}

void EncoderController::deactivate()
{
    if (!m_active)
        return;

    if (m_samd21Reader) {
        m_samd21Reader->close();
        delete m_samd21Reader;
        m_samd21Reader = nullptr;
    }

    m_vectorStd = nullptr;
    m_active    = false;
    emit evDeactivated();
}

// ----------------------------------------------------------------------------

void EncoderController::setCalibration(qreal zeroMm, qreal realMm, qreal estimatedMm)
{
    m_zeroMm      = zeroMm;
    m_scaleFactor = (qAbs(estimatedMm) > 1e-9) ? realMm / estimatedMm : 1.0;
    m_calibrated  = true;
    setCalibrationRealMm(realMm);
    setCalibrationEstimatedMm(estimatedMm);
    Logger::logInfo(QString("[EncoderController] calibration set: zero=%1 mm  real=%2 mm  estimated=%3 mm  scale=%4")
                    .arg(zeroMm,      0, 'f', 3)
                    .arg(realMm,      0, 'f', 3)
                    .arg(estimatedMm, 0, 'f', 3)
                    .arg(m_scaleFactor, 0, 'f', 6));
}

void EncoderController::calibrateNow(qreal realMm, qreal estimatedMm)
{
    setCalibrationRealMm(realMm);
    setCalibrationEstimatedMm(estimatedMm);
    setCalibration(m_lastRawMm, realMm, estimatedMm);
}

void EncoderController::resetCalibration()
{
    m_zeroMm      = 0.0;
    m_scaleFactor = 1.0;
    m_calibrated  = false;
    setCalibrationRealMm(100.0);
    setCalibrationEstimatedMm(100.0);
    Logger::logInfo("[EncoderController] calibration reset (identity)");
}

qreal EncoderController::calibratedPosition(qreal rawMm) const
{
    return (rawMm - m_zeroMm) * m_scaleFactor;
}

// ----------------------------------------------------------------------------

void EncoderController::onPositionMm(qreal mm)
{
    ++m_positionCount;
    m_lastRawMm = mm;
    qreal cal = calibratedPosition(mm);

    if (m_positionCount == 1 || m_positionCount % 200 == 0)
        Logger::logInfo(QString("[EncoderController] pos #%1  raw=%2 mm  cal=%3 mm")
                        .arg(m_positionCount).arg(mm, 0, 'f', 3).arg(cal, 0, 'f', 3));

    if (m_vectorStd && m_vectorStd->funcPosition())
        m_vectorStd->funcPosition()->injectEncoderPositionMm(cal);
}

void EncoderController::onReaderError(const QString& msg)
{
    emit evError(msg);
}
