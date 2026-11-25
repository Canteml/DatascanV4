/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : SerialPortRouter.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/10/2026
#
# ==========================================================================*/

#include <encoder/SerialPortRouter.h>
#include <encoder/EncoderController.h>
#include <altiscan/AltiscanController.h>
#include <v2/vector/VectorStd.h>
#include <v2/com/ComLinkManager.h>
#include <v2/prot/ProtMsg.h>
#include <log/Logger.h>

#include <QSerialPortInfo>
#include <QSettings>
#include <QVariantMap>
#include <algorithm>

// ----------------------------------------------------------------------------

static const char* SETTINGS_KEY_BACKUP_PORT = "BackupSerial/Port";

SerialPortRouter::SerialPortRouter(VectorStd* vectorStd,
                                   ComLinkManager* linkManager,
                                   AltiscanController* altiscan,
                                   EncoderController* encoder,
                                   QObject* parent)
    : QObject(parent)
    , m_vectorStd(vectorStd)
    , m_linkManager(linkManager)
    , m_altiscan(altiscan)
    , m_encoder(encoder)
    , m_timRetry(this)
{
    QSettings settings("Roboplanet", "DatascanV4");
    m_portName = settings.value(SETTINGS_KEY_BACKUP_PORT, QString()).toString();

    m_timRetry.setInterval(RETRY_PERIOD_MS);
    m_timRetry.setSingleShot(false);
    connect(&m_timRetry, &QTimer::timeout, this, &SerialPortRouter::onRetry);

    connect(m_vectorStd, &VectorStd::prodIdChanged, this, &SerialPortRouter::onProdIdChanged);
}

void SerialPortRouter::setPortName(const QString& value)
{
    if (m_portName == value)
        return;

    m_portName = value;
    QSettings settings("Roboplanet", "DatascanV4");
    settings.setValue(SETTINGS_KEY_BACKUP_PORT, m_portName);
    Logger::logInfo(QString("[SerialPortRouter] backup serial port set to '%1'").arg(m_portName));
    emit portNameChanged(m_portName);

    // Force a full re-assignment on the new port
    deactivateAll();
    m_target = TARGET_NONE;
    apply();
}

QVariantList SerialPortRouter::availablePorts() const
{
    static constexpr quint16 VID_INTEL = 0x8086;

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    std::sort(ports.begin(), ports.end(),
              [](const QSerialPortInfo& a, const QSerialPortInfo& b) {
                  return a.portName().mid(3).toInt() < b.portName().mid(3).toInt();
              });

    QVariantList list;
    QVariantMap none;
    none["address"] = QString();
    none["text"]    = tr("Aucun");
    list.append(none);

    bool currentListed = m_portName.isEmpty();
    for (const QSerialPortInfo& portInfo : ports) {
        if (portInfo.hasVendorIdentifier() && portInfo.vendorIdentifier() == VID_INTEL)
            continue;

        QVariantMap entry;
        entry["address"] = portInfo.portName();
        entry["text"]    = portInfo.description().isEmpty()
                           ? portInfo.portName()
                           : QString("%1 - %2").arg(portInfo.portName(), portInfo.description());
        list.append(entry);
        if (portInfo.portName() == m_portName)
            currentListed = true;
    }

    // Keep the saved port selectable even when the device is unplugged
    if (!currentListed) {
        QVariantMap entry;
        entry["address"] = m_portName;
        entry["text"]    = QString("%1 - %2").arg(m_portName, tr("non connecté"));
        list.append(entry);
    }
    return list;
}

// ----------------------------------------------------------------------------

SerialPortRouter::Target SerialPortRouter::targetForProduct(quint32 prodId) const
{
    switch (prodId) {
    case ProtMsg::PROD_UT_POLE:
        return TARGET_ENCODER;
    case ProtMsg::PROD_ALTISCAN_V2:
    case ProtMsg::PROD_ALTISCAN_V14:
        return TARGET_ALTISCAN;
    default:
        return TARGET_NONE;
    }
}

bool SerialPortRouter::isTargetActive() const
{
    switch (m_target) {
    case TARGET_ENCODER:  return m_encoder->isActive();
    case TARGET_ALTISCAN: return m_altiscan->isActive();
    default:              return true;
    }
}

void SerialPortRouter::deactivateAll()
{
    m_timRetry.stop();
    m_encoder->deactivate();
    m_altiscan->deactivate();
}

void SerialPortRouter::apply()
{
    const Target target = m_portName.isEmpty()
                          ? TARGET_NONE
                          : targetForProduct(m_vectorStd->getProdId());

    if (target == m_target && isTargetActive())
        return;     // nothing changed

    deactivateAll();
    m_target = target;

    // Reserve the port only when a legacy controller needs it, so that the
    // v2 discovery stays free to use it otherwise
    m_linkManager->setReservedSerialPorts(target == TARGET_NONE ? QStringList()
                                                                : QStringList{ m_portName });

    if (target == TARGET_NONE) {
        Logger::logInfo("[SerialPortRouter] backup serial port not used by the current product");
        return;
    }

    if (!tryActivate())
        m_timRetry.start();
}

bool SerialPortRouter::tryActivate()
{
    // Do not try (and log errors) while the device is unplugged
    bool present = false;
    for (const QSerialPortInfo& p : QSerialPortInfo::availablePorts()) {
        if (p.portName() == m_portName) {
            present = true;
            break;
        }
    }
    if (!present)
        return false;

    bool ok = false;
    if (m_target == TARGET_ENCODER)
        ok = m_encoder->activate(m_vectorStd, m_portName);
    else if (m_target == TARGET_ALTISCAN)
        ok = m_altiscan->activate(m_vectorStd, m_portName);

    if (ok)
        Logger::logInfo(QString("[SerialPortRouter] %1 active on %2")
                        .arg(m_target == TARGET_ENCODER ? "UT POLE encoder" : "Altiscan", m_portName));
    return ok;
}

void SerialPortRouter::onProdIdChanged(quint32 /*prodId*/)
{
    apply();
}

void SerialPortRouter::onRetry()
{
    if (m_target == TARGET_NONE || isTargetActive() || tryActivate())
        m_timRetry.stop();
}
