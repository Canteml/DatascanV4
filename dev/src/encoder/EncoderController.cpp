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
#include <v2/prot/ProtMsg.h>

#include <QSerialPortInfo>
#include <QSettings>
#include <QVariantMap>
#include <algorithm>

// ----------------------------------------------------------------------------

static const char* SETTINGS_KEY_BAUD_RATE        = "Encoder/BaudRate";
static const char* SETTINGS_KEY_BSCAN_SOURCE     = "BScan/Source";
static const char* SETTINGS_KEY_WHEEL_DIAMETER   = "WheelDiameterMm";
static const char* SETTINGS_KEY_CODER_RESOLUTION = "CoderResolution";
static const char* SETTINGS_KEY_ENCODER_REVERSE  = "EncoderReverse";

namespace {
    constexpr qreal PI_VALUE = 3.14159265358979323846;
    constexpr qreal     ALTISCAN_DEFAULT_WHEEL_DIAMETER_MM  = 52.5;
    constexpr qint32    ALTISCAN_DEFAULT_CODER_RESOLUTION   = 1024;
    constexpr qreal     UTPOLE_DEFAULT_WHEEL_DIAMETER_MM    = 56.8;
    constexpr qint32    UTPOLE_DEFAULT_CODER_RESOLUTION     = 82084;
}

EncoderController::EncoderController(QObject* parent)
    : QObject(parent)
{
    QSettings settings("Roboplanet", "DatascanV3");
    m_baudRate    = settings.value(SETTINGS_KEY_BAUD_RATE, m_baudRate).toInt();
    m_bscanSource = settings.value(SETTINGS_KEY_BSCAN_SOURCE, VectorFuncPosition::BSCAN_SOURCE_ALL).toString();
}

void EncoderController::setPositionVector(VectorStd* vectorStd)
{
    if (m_positionVector)
        disconnect(m_positionVector, &VectorStd::prodIdChanged, this, &EncoderController::onProdIdChanged);

    m_positionVector = vectorStd;
    if (m_positionVector) {
        connect(m_positionVector, &VectorStd::prodIdChanged, this, &EncoderController::onProdIdChanged);
        if (m_positionVector->funcPosition())
            m_positionVector->funcPosition()->setBScanSource(m_bscanSource);
    }
    loadOdometry();
}

void EncoderController::setBScanSource(const QString& value)
{
    if (m_bscanSource == value)
        return;

    m_bscanSource = value;
    QSettings settings("Roboplanet", "DatascanV3");
    settings.setValue(SETTINGS_KEY_BSCAN_SOURCE, m_bscanSource);
    if (m_positionVector && m_positionVector->funcPosition())
        m_positionVector->funcPosition()->setBScanSource(m_bscanSource);
    emit bscanSourceChanged(m_bscanSource);
}

void EncoderController::setBaudRate(qint32 value)
{
    if (value <= 0 || m_baudRate == value)
        return;

    m_baudRate = value;
    QSettings settings("Roboplanet", "DatascanV3");
    settings.setValue(SETTINGS_KEY_BAUD_RATE, m_baudRate);
    Logger::logInfo(QString("[EncoderController] baud rate set to %1").arg(m_baudRate));
    emit baudRateChanged(m_baudRate);

    // Reopen the active link with the new baud rate
    if (m_active && m_samd21Reader) {
        m_samd21Reader->close();
        if (m_samd21Reader->open(m_portName, m_baudRate))
            Logger::logInfo(QString("[EncoderController] UT POLE reader reopened on %1 @ %2").arg(m_portName).arg(m_baudRate));
    }
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
    QVariantList list;

    QVariantMap all;
    all["address"] = VectorFuncPosition::BSCAN_SOURCE_ALL;
    all["text"]    = tr("TOUTES LES SOURCES");
    list.append(all);

    QVariantMap serial;
    serial["address"] = VectorFuncPosition::BSCAN_SOURCE_SERIAL;
    serial["text"]    = tr("PORTEUR");
    list.append(serial);

    bool currentListed = (m_bscanSource == VectorFuncPosition::BSCAN_SOURCE_ALL
                          || m_bscanSource == VectorFuncPosition::BSCAN_SOURCE_SERIAL);

    if (m_linkManager) {
        const QStringList linkNames = m_linkManager->getAuthentifiedLinkNames();
        for (const QString& name : linkNames) {
            QVariantMap entry;
            entry["address"] = name;
            entry["text"]    = name;
            list.append(entry);
            if (name == m_bscanSource)
                currentListed = true;
        }
    }

    // Keep the saved network source selectable while it is disconnected
    if (!currentListed) {
        QVariantMap entry;
        entry["address"] = m_bscanSource;
        entry["text"]    = QString("%1 - %2").arg(m_bscanSource, tr("non connecté"));
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
    m_samd21Reader->setPulsesPerMm(utPolePulsesPerMm());

    connect(m_samd21Reader, &UtPoleEncoderReader::evPositionMm, this, &EncoderController::onPositionMm);
    connect(m_samd21Reader, &UtPoleEncoderReader::evError,      this, &EncoderController::onReaderError);
    if (!m_samd21Reader->open(portName, m_baudRate)) {
        delete m_samd21Reader;
        m_samd21Reader = nullptr;
        return false;
    }
    Logger::logInfo(QString("[EncoderController] UT POLE reader active on %1 @ %2").arg(portName).arg(m_baudRate));

    m_portName = portName;
    m_active   = true;
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
    if (m_encoderReverse)
        mm = -mm;

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

bool EncoderController::isUtPoleProfile() const
{
    return m_positionVector && m_positionVector->getProdId() == ProtMsg::PROD_UT_POLE;
}

QString EncoderController::odometryKey(const char* name) const
{
    return QString("Encoder/%1/%2").arg(isUtPoleProfile() ? "UtPole" : "Altiscan", name);
}

qreal EncoderController::utPolePulsesPerMm() const
{
    return static_cast<qreal>(m_coderResolution) / (PI_VALUE * m_wheelDiameterMm);
}

void EncoderController::setWheelDiameterMm(qreal value)
{
    value = qRound(qBound(1.0, value, 1000.0) * 100.0) / 100.0;   // 0.01 mm, avoids float drift on +/- 0.1
    if (qFuzzyCompare(m_wheelDiameterMm, value))
        return;

    m_wheelDiameterMm = value;
    QSettings settings("Roboplanet", "DatascanV3");
    settings.setValue(odometryKey(SETTINGS_KEY_WHEEL_DIAMETER), m_wheelDiameterMm);
    emit wheelDiameterMmChanged(m_wheelDiameterMm);
    applyOdometry();
}

void EncoderController::setCoderResolution(qint32 value)
{
    value = qBound(1, value, 200000);
    if (m_coderResolution == value)
        return;

    m_coderResolution = value;
    QSettings settings("Roboplanet", "DatascanV3");
    settings.setValue(odometryKey(SETTINGS_KEY_CODER_RESOLUTION), m_coderResolution);
    emit coderResolutionChanged(m_coderResolution);
    applyOdometry();
}

void EncoderController::setEncoderReverse(bool value)
{
    if (m_encoderReverse == value)
        return;

    m_encoderReverse = value;
    QSettings settings("Roboplanet","DatascanV3");
    settings.setValue(odometryKey(SETTINGS_KEY_ENCODER_REVERSE), m_encoderReverse);
    emit encoderReverseChanged(m_encoderReverse);
}

void EncoderController::loadOdometry()
{
    const bool utPole = isUtPoleProfile();
    QSettings settings("Roboplanet", "DatascanV3");
    const qreal diameter = settings.value(odometryKey(SETTINGS_KEY_WHEEL_DIAMETER),
                                          utPole ? UTPOLE_DEFAULT_WHEEL_DIAMETER_MM
                                                 : ALTISCAN_DEFAULT_WHEEL_DIAMETER_MM).toDouble();
    const qint32 resolution = settings.value(odometryKey(SETTINGS_KEY_CODER_RESOLUTION),
                                             utPole ? UTPOLE_DEFAULT_CODER_RESOLUTION
                                                    : ALTISCAN_DEFAULT_CODER_RESOLUTION).toInt();

    if (!qFuzzyCompare(m_wheelDiameterMm, diameter)) {
        m_wheelDiameterMm = diameter;
        emit wheelDiameterMmChanged(m_wheelDiameterMm);
    }
    if (m_coderResolution != resolution) {
        m_coderResolution = resolution;
        emit coderResolutionChanged(m_coderResolution);
    }

    const bool reverse = settings.value(odometryKey(SETTINGS_KEY_ENCODER_REVERSE), false).toBool();
    m_encoderReverse = reverse;
    emit encoderReverseChanged(m_encoderReverse);
    applyOdometry();
}

void EncoderController::applyOdometry()
{
    if (m_samd21Reader)
        m_samd21Reader->setPulsesPerMm(utPolePulsesPerMm());

    Logger::logInfo(QString("[EncoderController] odometry (%1): wheel=%2 mm  resolution=%3")
                        .arg(isUtPoleProfile() ? "UT POLE" : "Altiscan")
                        .arg(m_wheelDiameterMm, 0, 'f', 2).arg(m_coderResolution));
    emit odometryChanged(m_wheelDiameterMm, m_coderResolution);
}

void EncoderController::onProdIdChanged(quint32 /*prodId*/)
{
    loadOdometry();     // switch to the values saved for the new product
}
