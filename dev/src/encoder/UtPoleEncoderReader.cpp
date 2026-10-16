/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UtPoleEncoderReader.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <encoder/UtPoleEncoderReader.h>
#include <log/Logger.h>

// ----------------------------------------------------------------------------

UtPoleEncoderReader::UtPoleEncoderReader(QObject* parent)
    : QObject(parent)
    , m_port(this)
{
    connect(&m_port, &QSerialPort::readyRead,
            this,   &UtPoleEncoderReader::onReadyRead);
    connect(&m_port, &QSerialPort::errorOccurred,
            this,   &UtPoleEncoderReader::onSerialError);
}

bool UtPoleEncoderReader::open(const QString& portName)
{
    m_port.setPortName(portName);
    m_port.setBaudRate(9600);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::NoParity);
    m_port.setStopBits(QSerialPort::OneStop);
    // Do NOT call setFlowControl(NoFlowControl): on Windows that sets
    // fDtrControl=DTR_CONTROL_DISABLE in the DCB, keeping DTR low from
    // port-open and preventing the SAMD21 USB-CDC from sending real frames.
    // Leave the Windows DCB default (DTR_CONTROL_ENABLE = DTR high).

    if (!m_port.open(QIODevice::ReadWrite)) {
        emit evError(QString("Cannot open %1: %2").arg(portName, m_port.errorString()));
        return false;
    }

    // Flush the stale zero-filled USB CDC buffer, then confirm DTR is high.
    m_port.clear(QSerialPort::Input);
    m_port.setDataTerminalReady(true);

    m_buffer.clear();
    m_firstFrame   = true;
    m_lastRaw      = INIT_RAW;
    m_accumulated  = INIT_RAW;
    return true;
}

void UtPoleEncoderReader::close()
{
    if (m_port.isOpen())
        m_port.close();
    m_buffer.clear();
}

bool UtPoleEncoderReader::isOpen() const
{
    return m_port.isOpen();
}

// ----------------------------------------------------------------------------

void UtPoleEncoderReader::onReadyRead()
{
    QByteArray incoming = m_port.readAll();
    m_buffer.append(incoming);

    while (m_buffer.size() >= FRAME_SIZE) {
        // Sync to header byte
        int idx = m_buffer.indexOf(char(FRAME_HEADER));
        if (idx < 0) {
            Logger::logInfo(QString("[UtPoleEncoderReader] no header 0x23 in %1 bytes, clearing").arg(m_buffer.size()));
            m_buffer.clear();
            return;
        }
        if (idx > 0)
            m_buffer.remove(0, idx);

        if (m_buffer.size() < FRAME_SIZE)
            return;

        const QByteArray frame = m_buffer.left(FRAME_SIZE);

        // Validate fixed bytes
        if (static_cast<quint8>(frame[1]) != 0x01 ||
            static_cast<quint8>(frame[2]) != 0x02 ||
            static_cast<quint8>(frame[3]) != 0x63) {
            Logger::logInfo(QString("[UtPoleEncoderReader] validation fail: [1]=0x%1 [2]=0x%2 [3]=0x%3")
                            .arg((quint8)frame[1], 2, 16, QChar('0'))
                            .arg((quint8)frame[2], 2, 16, QChar('0'))
                            .arg((quint8)frame[3], 2, 16, QChar('0')));
            m_buffer.remove(0, 1);
            continue;
        }

        // Decode 24-bit position (little-endian bytes 6, 7, 8)
        int32_t raw = static_cast<quint8>(frame[6])
                    | (static_cast<quint8>(frame[7]) << 8)
                    | (static_cast<quint8>(frame[8]) << 16);

        if (m_firstFrame) {
            m_lastRaw     = raw;
            m_accumulated = raw;
            m_firstFrame  = false;
        } else {
            // Handle 24-bit wrap-around
            int32_t delta = raw - m_lastRaw;
            if (delta >  8388608) delta -= 16777216;
            if (delta < -8388608) delta += 16777216;
            m_lastRaw      = raw;
            m_accumulated += delta;
        }

        // 46 units = 0.1 mm  →  460 units = 1 mm
        qreal mm = static_cast<qreal>(m_accumulated - INIT_RAW) / 460.0;

        ++m_frameCount;
        if (m_frameCount == 1 || m_frameCount % 100 == 0)
            Logger::logInfo(QString("[UtPoleEncoderReader] frame #%1  raw=0x%2  pos=%3 mm")
                            .arg(m_frameCount)
                            .arg(raw, 6, 16, QChar('0'))
                            .arg(mm, 0, 'f', 3));

        emit evPositionMm(mm);

        m_buffer.remove(0, FRAME_SIZE);
    }
}

void UtPoleEncoderReader::onSerialError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
        emit evError(m_port.errorString());
}
