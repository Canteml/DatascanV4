
/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UtPoleEncoderReader.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 20/06/2026
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QSerialPort>

// ----------------------------------------------------------------------------
// -- UtPoleEncoderReader
//
// Reads 14-byte binary frames from an Atmel SAMD21 USB encoder (COM port)
// and emits evPositionMm() for each valid frame.
//
// Frame layout (9600 8N1):
//   [0]  = 0x23  (sync header)
//   [1]  = 0x01  [2] = 0x02  [3] = 0x63  (fixed validation bytes)
//   [4]  = 0x00  [5] = 0x00
//   [6][7][8]    = 24-bit position, little-endian  (46 units = 0.1 mm)
//   [9]  = 0x00
//   [10][11][12] = duplicate of [6][7][8]
//   [13] = end byte
//
// Initial position (0 mm): 0x7F1EFF (= 8331007)
// Scale: positionMm = (accumulated - 0x7F1EFF) / 460.0

class UtPoleEncoderReader : public QObject
{
    Q_OBJECT

public:
    explicit UtPoleEncoderReader(QObject* parent = nullptr);

    bool open(const QString& portName);
    void close();
    bool isOpen() const;

signals:
    void evPositionMm(qreal mm);
    void evError(const QString& msg);

private slots:
    void onReadyRead();
    void onSerialError(QSerialPort::SerialPortError error);

private:
    static constexpr int     FRAME_SIZE   = 14;
    static constexpr quint8  FRAME_HEADER = 0x23;
    static constexpr int32_t INIT_RAW     = 0x7F1EFF;  // raw value at 0 mm

    QSerialPort m_port;
    QByteArray  m_buffer;

    int32_t  m_lastRaw     = INIT_RAW;
    int64_t  m_accumulated = INIT_RAW;
    bool     m_firstFrame  = true;
    quint32  m_frameCount  = 0;
};
