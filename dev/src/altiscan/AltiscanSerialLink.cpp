/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : AltiscanSerialLink.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 21/07/2026
#
# ==========================================================================*/

#include <altiscan/AltiscanSerialLink.h>
#include <log/Logger.h>

namespace {
    constexpr qreal PI_VALUE = 3.14159265358979323846;
}

// ----------------------------------------------------------------------------

AltiscanSerialLink::AltiscanSerialLink(QObject* parent)
    : QObject(parent)
    , m_port(this)
{
    char f[7] = { (char)SYNC_BYTE, (char)0x02, (char)0x01, (char)CMD_POSITION, (char)0x00, (char)0x01, (char)0x00 };
    m_frameRequestPosition.append(f, 7);

    char j[8] = { (char)SYNC_BYTE, (char)0x02, (char)0x01, (char)CMD_JOYSTICK, (char)0x00, (char)0x02, (char)STOP_BYTE, (char)STOP_BYTE };
    m_frameJoystick.append(j, 8);

    m_coefStepToMm = (PI_VALUE * DEFAULT_WHEEL_DIAMETER_MM)
                    / static_cast<qreal>(DEFAULT_CODER_RESOLUTION * DEFAULT_MOTOR_REDUCTION);

    m_timAutoSend.setInterval(AUTO_SEND_MS);
    m_timAutoSend.setSingleShot(false);
    connect(&m_timAutoSend, &QTimer::timeout, this, &AltiscanSerialLink::onAutoSendTimeout);

    m_timReconnect.setInterval(RECONNECT_INTERVAL_MS);
    m_timReconnect.setSingleShot(false);
    connect(&m_timReconnect, &QTimer::timeout, this, &AltiscanSerialLink::onReconnectTimeout);

    connect(&m_port, &QSerialPort::readyRead,     this, &AltiscanSerialLink::onReadyRead);
    connect(&m_port, &QSerialPort::errorOccurred, this, &AltiscanSerialLink::onSerialError);
}

// ----------------------------------------------------------------------------

bool AltiscanSerialLink::open(const QString& portName)
{
    if (m_port.isOpen())
        return true;

    m_portName = portName;

    if (!tryOpenPort(portName)) {
        emit evError(QString("Cannot open %1: %2").arg(portName, m_port.errorString()));
        return false;
    }

    Logger::logInfo(QString("[AltiscanSerialLink] opened %1 @ 115200 8N1").arg(portName));
    onConnectionEstablished();
    return true;
}

void AltiscanSerialLink::close()
{
    if (m_timAutoSend.isActive())
        m_timAutoSend.stop();
    if (m_timReconnect.isActive())
        m_timReconnect.stop();

    if (m_port.isOpen()) {
        m_port.close();
        Logger::logInfo("[AltiscanSerialLink] closed");
        emit evConnectionStateChanged(false);
    }

    m_rxBuffer.clear();
}

bool AltiscanSerialLink::isOpen() const
{
    return m_port.isOpen();
}

bool AltiscanSerialLink::tryOpenPort(const QString& portName)
{
    m_port.setPortName(portName);
    m_port.setBaudRate(115200);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::NoParity);
    m_port.setStopBits(QSerialPort::OneStop);
    return m_port.open(QIODevice::ReadWrite);
}

void AltiscanSerialLink::onConnectionEstablished()
{
    m_port.clear(QSerialPort::Input);
    m_rxBuffer.clear();
    m_lastPositionRaw = 0;
    m_requestCount    = 0;
    m_replyCount      = 0;

    emit evConnectionStateChanged(true);
    m_timAutoSend.start();
}

void AltiscanSerialLink::handleFatalError(const QString& reason)
{
    if (!m_port.isOpen() && m_timReconnect.isActive())
        return; // already in a reconnect cycle, nothing new to do

    if (m_timAutoSend.isActive())
        m_timAutoSend.stop();
    if (m_port.isOpen())
        m_port.close();
    m_rxBuffer.clear();

    Logger::logError(QString("[AltiscanSerialLink] connection lost on %1 (%2) - retrying every %3ms")
                     .arg(m_portName, reason).arg(RECONNECT_INTERVAL_MS));
    emit evConnectionStateChanged(false);

    m_reconnectAttempts = 0;
    m_timReconnect.start();
}

void AltiscanSerialLink::onReconnectTimeout()
{
    ++m_reconnectAttempts;

    if (!tryOpenPort(m_portName)) {
        if (m_reconnectAttempts == 1 || m_reconnectAttempts % 10 == 0) {
            Logger::logInfo(QString("[AltiscanSerialLink] reconnect attempt #%1 on %2 failed: %3")
                            .arg(m_reconnectAttempts).arg(m_portName, m_port.errorString()));
        }
        return; // m_timReconnect keeps repeating
    }

    m_timReconnect.stop();
    Logger::logInfo(QString("[AltiscanSerialLink] reconnected to %1 after %2 attempt(s)")
                    .arg(m_portName).arg(m_reconnectAttempts));
    onConnectionEstablished();
}

void AltiscanSerialLink::setCoefStepToMm(qreal mmPerPulse)
{
    m_coefStepToMm = mmPerPulse;
}

void AltiscanSerialLink::setOdometry(qreal wheelDiameterMm, quint32 coderResolution)
{
    if (wheelDiameterMm <= 0.0 || 0 == coderResolution)
        return;

    setCoefStepToMm((PI_VALUE * wheelDiameterMm) / static_cast<qreal>(coderResolution * DEFAULT_MOTOR_REDUCTION));

    Logger::logInfo(QString("[AltiscanSerialLink] odometry: wheel = %1 mm resolution = %2 coef = %3 mm/pulse")
                        .arg(wheelDiameterMm, 0, 'f', 2).arg(coderResolution).arg(m_coefStepToMm, 0, 'f',6));
}

void AltiscanSerialLink::setJoystick(qint8 x, qint8 y)
{
    m_joyX = x;
    m_joyY = y;
}

// ----------------------------------------------------------------------------

void AltiscanSerialLink::onAutoSendTimeout()
{
    sendRequestPosition();
    // Non-blocking equivalent of the legacy QThread::msleep(10) between the
    // two writes - this class runs on the GUI thread and must not stall it.
    QTimer::singleShot(INTER_FRAME_MS, this, &AltiscanSerialLink::sendJoystickFrame);
}

void AltiscanSerialLink::sendRequestPosition()
{
    if (!m_port.isOpen())
        return;

    m_port.write(m_frameRequestPosition);
    ++m_requestCount;
}

void AltiscanSerialLink::sendJoystickFrame()
{
    if (!m_port.isOpen())
        return;

    // MOVE: 127 - y (positive y = "forward" key/stick = wheels turn the
    // opposite way), clamped to a byte. Sign confirmed inverted from the
    // naive mapping by physical testing on the real robot: pressing the
    // forward control must rotate the wheels (and consequently the B-scan
    // position graph, which tracks the physical encoder) the other way.
    // TURN: 127 +/- x/2 (positive x = right); the sign flips while moving
    // (y != 0) - this is a confirmed legacy firmware/drivetrain quirk
    // ("diagonals inverted"), not a bug, and must be preserved as-is. The
    // move/no-move condition below uses the raw joystick y (not the
    // inverted moveRaw) since it only needs to know whether there is any
    // forward/backward input at all, not its (now-flipped) sign.
    const qint32 moveRaw = static_cast<qint32>(STOP_BYTE) - m_joyY;
    const qint32 turnComponent = (0 != m_joyY) ? -(m_joyX / 2) : (m_joyX / 2);
    const qint32 turnRaw = static_cast<qint32>(STOP_BYTE) + turnComponent;

    const quint8 moveByte = static_cast<quint8>(qBound(0, moveRaw, 255));
    const quint8 turnByte = static_cast<quint8>(qBound(0, turnRaw, 255));

    m_frameJoystick[6] = static_cast<char>(moveByte);
    m_frameJoystick[7] = static_cast<char>(turnByte);

    m_port.write(m_frameJoystick);

    ++m_joystickSendCount;
    if (m_joystickSendCount == 1 || m_joystickSendCount % 50 == 0) {
        Logger::logInfo(QString("[AltiscanSerialLink] joystick #%1  x=%2 y=%3  MOVE=%4 TURN=%5")
                        .arg(m_joystickSendCount).arg(m_joyX).arg(m_joyY).arg(moveByte).arg(turnByte));
    }
}

// ----------------------------------------------------------------------------

void AltiscanSerialLink::onReadyRead()
{
    m_lastRxChunk = m_port.readAll();
    m_rxBuffer.append(m_lastRxChunk);
    processBuffer();
}

void AltiscanSerialLink::processBuffer()
{
    for (;;) {
        const int syncIdx = m_rxBuffer.indexOf(static_cast<char>(SYNC_BYTE));
        if (syncIdx < 0) {
            // DEBUG : bytes dropped while looking for the sync byte
            Logger::logWarning(QString("[AltiscanSerialLink] no sync, dropped %1 bytes : %2")
                               .arg(m_rxBuffer.size()).arg(QString(m_rxBuffer.toHex(' '))));
            m_rxBuffer.clear();
            return;
        }
        if (syncIdx > 0) {
            Logger::logWarning(QString("[AltiscanSerialLink] dropped %1 bytes before sync : %2")
                               .arg(syncIdx).arg(QString(m_rxBuffer.left(syncIdx).toHex(' '))));
            m_rxBuffer.remove(0, syncIdx);
        }

        if (m_rxBuffer.size() < HEADER_SIZE)
            return; // wait for the rest of the header

        const quint8 src = static_cast<quint8>(m_rxBuffer[1]);
        const quint8 dst = static_cast<quint8>(m_rxBuffer[2]);
        const quint8 cmd = static_cast<quint8>(m_rxBuffer[3]);
        // m_rxBuffer[4] = FLAGS, always 0x00 in every legacy frame observed
        const quint8 len = static_cast<quint8>(m_rxBuffer[5]);

        if (src != SRC_ROBOT || dst != DST_SOFTWARE) {
            // Not a valid header at this sync position - drop the sync byte
            // and look for the next one instead of discarding everything.
            Logger::logWarning(QString("[AltiscanSerialLink] invalid header, resync : %1")
                               .arg(QString(m_rxBuffer.left(HEADER_SIZE).toHex(' '))));
            m_rxBuffer.remove(0, 1);
            continue;
        }

        const int frameSize = HEADER_SIZE + len;
        if (m_rxBuffer.size() < frameSize)
            return; // wait for the rest of the payload

        const QByteArray payload = m_rxBuffer.mid(HEADER_SIZE, len);
        m_currentFrame = m_rxBuffer.left(frameSize);
        handleFrame(cmd, payload);

        m_rxBuffer.remove(0, frameSize);
    }
}

void AltiscanSerialLink::handleFrame(quint8 cmd, const QByteArray& payload)
{
    if (CMD_POSITION == cmd) {
        if (payload.size() < 4) {
            Logger::logInfo(QString("[AltiscanSerialLink] 'r' reply too short (%1 bytes), ignored").arg(payload.size()));
            return;
        }

        const quint8 b0 = static_cast<quint8>(payload[0]);
        const quint8 b1 = static_cast<quint8>(payload[1]);
        const quint8 b2 = static_cast<quint8>(payload[2]);
        const quint8 b3 = static_cast<quint8>(payload[3]);
        const qint32 posInt = static_cast<qint32>(
            static_cast<quint32>(b0)
            | (static_cast<quint32>(b1) << 8)
            | (static_cast<quint32>(b2) << 16)
            | (static_cast<quint32>(b3) << 24));

        // DEBUG : dump the raw frame when the payload size is unexpected or the position jumps
        if (4 != payload.size() || (m_replyCount > 0 && qAbs(static_cast<qint64>(posInt) - m_lastPositionRaw) > 20000)) {
            Logger::logWarning(QString("[AltiscanSerialLink] suspicious 'r' reply #%1 : raw=%2 (last %3) len=%4 | frame : %5 | rx chunk : %6 | buffer : %7")
                               .arg(m_replyCount + 1).arg(posInt).arg(m_lastPositionRaw).arg(payload.size())
                               .arg(QString(m_currentFrame.toHex(' ')), QString(m_lastRxChunk.toHex(' ')), QString(m_rxBuffer.toHex(' '))));
        }

        m_lastPositionRaw = posInt;
        ++m_replyCount;

        const qreal mm = static_cast<qreal>(posInt) * -1.0 * m_coefStepToMm;

        if (m_replyCount == 1 || m_replyCount % 50 == 0) {
            Logger::logInfo(QString("[AltiscanSerialLink] reply #%1 raw=%2 pulses  pos=%3 mm")
                            .arg(m_replyCount).arg(posInt).arg(mm, 0, 'f', 3));
        }

        emit evPositionRaw(posInt);
        emit evPositionMm(mm);

    } else if (CMD_PRESSURE == cmd) {
        if (payload.size() < 2) {
            Logger::logInfo(QString("[AltiscanSerialLink] 'p' reply too short (%1 bytes), ignored").arg(payload.size()));
            return;
        }

        const quint8 b0 = static_cast<quint8>(payload[0]);
        const quint8 b1 = static_cast<quint8>(payload[1]);
        const quint16 pressInt = static_cast<quint16>(b0 | (b1 << 8));

        Logger::logInfo(QString("[AltiscanSerialLink] pressure = %1 mBar").arg(pressInt));
        emit evPressureRaw(pressInt);

    } else if (CMD_ORIENTATION == cmd) {
        if (payload.size() < 12) {
            Logger::logInfo(QString("[AltiscanSerialLink] 'o' reply too short (%1 bytes), ignored").arg(payload.size()));
            return;
        }

        const qint16 aX = static_cast<qint16>(static_cast<quint8>(payload[0])  | (static_cast<quint8>(payload[1])  << 8));
        const qint16 aY = static_cast<qint16>(static_cast<quint8>(payload[2])  | (static_cast<quint8>(payload[3])  << 8));
        const qint16 aZ = static_cast<qint16>(static_cast<quint8>(payload[4])  | (static_cast<quint8>(payload[5])  << 8));
        const qint16 oX = static_cast<qint16>(static_cast<quint8>(payload[6])  | (static_cast<quint8>(payload[7])  << 8));
        const qint16 oY = static_cast<qint16>(static_cast<quint8>(payload[8])  | (static_cast<quint8>(payload[9])  << 8));
        const qint16 oZ = static_cast<qint16>(static_cast<quint8>(payload[10]) | (static_cast<quint8>(payload[11]) << 8));

        ++m_inertialCount;
        if (m_inertialCount == 1 || m_inertialCount % 50 == 0) {
            Logger::logInfo(QString("[AltiscanSerialLink] orientation #%1  acc=(%2,%3,%4)  raw=(%5,%6,%7)")
                            .arg(m_inertialCount).arg(aX).arg(aY).arg(aZ).arg(oX).arg(oY).arg(oZ));
        }

        emit evInertialRaw(aX, aY, aZ, oX, oY, oZ);

    } else {
        Logger::logInfo(QString("[AltiscanSerialLink] unhandled cmd 0x%1 (%2 payload bytes) - not wired yet")
                        .arg(cmd, 2, 16, QChar('0')).arg(payload.size()));
    }
}

void AltiscanSerialLink::onSerialError(QSerialPort::SerialPortError error)
{
    if (QSerialPort::NoError == error)
        return;

    emit evError(m_port.errorString());

    switch (error) {
        case QSerialPort::ResourceError:      // most common on Windows when a USB/serial device is unplugged
        case QSerialPort::DeviceNotFoundError:
        case QSerialPort::ReadError:
        case QSerialPort::WriteError:
            handleFatalError(m_port.errorString());
            break;
        default:
            // Transient/non-fatal (e.g. ParityError on a single corrupted
            // byte) - already reported via evError above, connection stays
            // up and the next 100ms cycle proceeds normally.
            break;
    }
}
