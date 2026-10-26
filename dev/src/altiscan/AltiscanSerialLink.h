/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : AltiscanSerialLink.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 21/07/2026
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QSerialPort>
#include <QTimer>

// ----------------------------------------------------------------------------
// -- AltiscanSerialLink
//
// Legacy Roboplanet frame protocol over RS-232 (115200 8N1), as implemented
// by the original Datascan vecAltiscan/vecLinear/vecLink classes. This is a
// dedicated, self-contained transport: it does NOT go through the v2 /
// ComLinkManager / ProtMsg layer (incompatible protocol, different baud
// rate), exactly like UtPoleEncoderReader is a dedicated channel for the
// USB encoder.
//
// Frame layout: [0x23][SRC][DST][CMD][FLAGS][LEN][payload: LEN bytes]
//   Software -> Robot : SRC=0x02 DST=0x01
//   Robot -> Software : SRC=0x01 DST=0x02
//
// Every 100ms: send the request-position frame ('r'), then ~10ms later the
// joystick frame ('v') with the last known move/turn bytes - same cadence
// and inter-frame delay as the legacy vecAltiscan/vecLink, confirmed
// necessary in the legacy source comments ("50ms and 200ms do not work",
// "do not change/move/delete this delay"). The 10ms gap is implemented as a
// non-blocking QTimer::singleShot() rather than the legacy's
// QThread::msleep(10), since this class runs on the GUI thread and must not
// stall it.
//
// The robot also sends 'p' (pressure) and 'o' (orientation/IMU) frames
// spontaneously when available (not requested on a timer, unlike 'r') -
// parsed and re-emitted as raw sensor values; unit/calibration conversion
// is left to the consumer (AltiscanController), matching how position is
// handled (this class only knows the wire format, not the app's sensor
// model).
//
// The 100ms cadence is a hard firmware constraint and is therefore owned by
// this class, not by an external driver - do not change it.
//
// Runtime disconnect handling: a fatal serial error (e.g. the USB/serial
// cable is unplugged while running) closes the port and starts a 500ms
// retry timer (same interval as the legacy vecLink::AUTO_CONNECT_DELAY)
// that keeps trying to reopen the same port until it succeeds - no app
// restart needed. This only applies after a successful connection is lost;
// it does not retry an initial open() failure (see open()'s own doc).

class AltiscanSerialLink : public QObject
{
    Q_OBJECT

public:
    explicit AltiscanSerialLink(QObject* parent = nullptr);

    bool open(const QString& portName = "COM6");
    void close();
    bool isOpen() const;

    void setCoefStepToMm(qreal mmPerPulse);
    qreal coefStepToMm() const { return m_coefStepToMm; }
    void setOdometry(qreal wheelDiameterMm, quint32 coderResolution);

    void setJoystick(qint8 x, qint8 y);

signals:
    void evPositionRaw(qint32 pulses);
    void evPositionMm(qreal mm);

    void evPressureRaw(quint16 milliBar);

    void evInertialRaw(qint16 aX, qint16 aY, qint16 aZ, qint16 oX, qint16 oY, qint16 oZ);

    void evError(const QString& msg);
    void evConnectionStateChanged(bool connected);

private slots:
    void onReadyRead();
    void onSerialError(QSerialPort::SerialPortError error);
    void onAutoSendTimeout();
    void sendJoystickFrame();
    void onReconnectTimeout();

private:
    void sendRequestPosition();
    void processBuffer();
    void handleFrame(quint8 cmd, const QByteArray& payload);
    bool tryOpenPort(const QString& portName);
    void handleFatalError(const QString& reason);
    void onConnectionEstablished();

    static constexpr quint8 SYNC_BYTE      = 0x23; // '#'
    static constexpr quint8 SRC_ROBOT      = 0x01;
    static constexpr quint8 DST_SOFTWARE   = 0x02;
    static constexpr quint8 CMD_POSITION    = 0x72; // 'r'
    static constexpr quint8 CMD_JOYSTICK    = 0x76; // 'v'
    static constexpr quint8 CMD_PRESSURE    = 0x70; // 'p'
    static constexpr quint8 CMD_ORIENTATION = 0x6F; // 'o'
    static constexpr quint8 STOP_BYTE      = 127;  // move/turn neutral value
    static constexpr int    HEADER_SIZE    = 6;    // # SRC DST CMD FLAGS LEN
    static constexpr int    AUTO_SEND_MS   = 100;  // firmware constraint, do not change
    static constexpr int    INTER_FRAME_MS = 10;   // firmware constraint, do not change
    static constexpr int    RECONNECT_INTERVAL_MS = 500; // matches legacy vecLink::AUTO_CONNECT_DELAY

    static constexpr qreal  DEFAULT_WHEEL_DIAMETER_MM = 52.5;
    static constexpr quint32 DEFAULT_CODER_RESOLUTION  = 1024;
    static constexpr quint32 DEFAULT_MOTOR_REDUCTION   = 103;

    QSerialPort m_port;
    QByteArray  m_rxBuffer;
    QTimer      m_timAutoSend;
    QTimer      m_timReconnect;
    QString     m_portName;
    QByteArray  m_frameRequestPosition;
    QByteArray  m_frameJoystick;

    qreal   m_coefStepToMm;
    qint32  m_lastPositionRaw = 0;
    QByteArray m_lastRxChunk;           // DEBUG : last bytes read from the port
    QByteArray m_currentFrame;          // DEBUG : whole frame being handled (header + payload)
    quint32 m_requestCount    = 0;
    quint32 m_replyCount      = 0;

    qint8   m_joyX = 0;
    qint8   m_joyY = 0;
    quint32 m_joystickSendCount = 0;
    quint32 m_inertialCount     = 0;
    quint32 m_reconnectAttempts = 0;
};
