/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyManager.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QVector>

class UsKeyDriver;
class UsKeyAcquisitionThread;
class UsbDeviceFilter;
class QTimer;
class QThread;
class QAbstractNativeEventFilter;

// ----------------------------------------------------------------------------
// -- UsKeyManager
//
// Manages the full lifecycle of the US-Key device:
//   - DLL loading (UsKeyDriver)
//   - Device connection / disconnection
//   - Parameter configuration (gain, voltage, gates, ...)
//   - Continuous A-scan acquisition (via internal thread)
//
// Usage:
//   UsKeyManager mgr;
//   mgr.initialize();          // loads ap_int_usb.dll
//   mgr.connectDevice(0);      // opens device 0
//   mgr.setGain(20);
//   mgr.startAcquisition();    // starts emitting evNewRawAScan
//   connect(&mgr, &UsKeyManager::evNewRawAScan, ...);

class UsKeyManager : public QObject
{
    Q_OBJECT

public:
    explicit UsKeyManager(QObject* parent = nullptr);
    ~UsKeyManager();

    // -- Lifecycle
    bool initialize(const QString& dllPath = "ap_int_usb");
    bool connectDevice(int deviceNum = 0);
    void disconnectDevice();
    bool isConnected() const;
    int  deviceNum() const;

    // -- Acquisition control
    // nbSamples: number of samples to request per A-scan (default 4000)
    // dataType:  0 = HF (raw RF), 1 = A-scan (rectified)
    // waveType:  0 = full rectified, 1 = negative, 2 = positive
    void startAcquisition(int nbSamples = 4000, int dataType = 0, int waveType = 0);
    void stopAcquisition();
    bool isAcquiring() const;

    // -- Transmitter parameters
    void setPrf(double kHz);
    void setVoltage(double volts);     // 30–230 V
    void setWidth(double width);       // 0–255
    void setPulseDelay(double step);   // step × 1.6 µs

    // -- Receiver parameters
    void setFilterMode(double filter, double mode);
    // filter: 0=1.25MHz 1=2.5MHz 2=5MHz 3=10MHz 4=BroadBand
    // mode:   0=T/R connected  1=T/R disconnected
    void setWave(double waveType);
    // waveType: 0=redressée  1=négative  2=positive
    void setGain(double dB);           // 0–79.9 dB
    void setScaleDelay(double us);
    void setSamplingFreqMode(double freq, double mode);
    // freq: 0=160MHz 1=80MHz 2=40MHz 3=20MHz

    // -- Gate parameters (gate = 1, 2 or 3)
    void setGatePosition(int gate, double us);
    void setGateWidth(int gate, double us);
    void setGateHeight(int gate, double percent);
    void setRelays(double bitmask);

signals:
    void evConnected();
    void evDisconnected();
    // Raw 16-bit samples (12-bit ADC values, convert to 8-bit with >> 4)
    void evNewRawAScan(QVector<unsigned short> data, qint64 timestampMs);
    void evError(const QString& msg);

private slots:
    void onDriverError(const QString& msg);
    void onDataLost();
    void onRetryTimeout();
    void onWatchdogTimeout();
    void onConnectAttemptFinished(bool found, bool configOk);

private:
    friend class UsbDeviceFilter;
    void startRetryTimer();

    // Runs UsKeyDriver::initUsb()/loadConfiguration() on m_connectThread.
    // These block on the vendor ap_int_usb.dll (and its 32-bit helper process)
    // for an unbounded time — observed up to 12+ s, sometimes longer — so they
    // must never run on the GUI thread. Result comes back via
    // onConnectAttemptFinished(), posted to this object's (GUI) thread.
    void startConnectAttempt();

    // Same reasoning as startConnectAttempt(): probes whether the device is
    // still present after a USB hotplug event, without blocking the GUI thread.
    void probeStillPresent();

    UsKeyDriver*              m_driver        = nullptr;
    UsKeyAcquisitionThread*   m_thread        = nullptr;
    QTimer*                   m_retryTimer    = nullptr;
    QTimer*                   m_watchdogTimer = nullptr;
    QAbstractNativeEventFilter* m_nativeFilter = nullptr;
    // Single persistent worker thread that m_driver lives on for the whole
    // lifetime of UsKeyManager. All initUsb()/loadConfiguration() probes
    // (startConnectAttempt/probeStillPresent) are posted to it via
    // QMetaObject::invokeMethod — never a freshly spawned thread per call.
    // A minidump showed the vendor DLL (built on the LabVIEW runtime, lvrt.dll)
    // crashing with an access violation after we called it from a
    // throwaway QThread::create() worker for every connect/probe attempt;
    // consolidating onto one long-lived thread avoids repeatedly handing the
    // runtime a new, short-lived thread identity to contend with.
    QThread*                  m_connectThread = nullptr;
    int                       m_deviceNum     = 0;
    bool                      m_connected     = false;
    bool                      m_connecting    = false;   ///< a background connect attempt is in flight
    int                       m_lastNbSamples = 4000;
    int                       m_lastDataType  = 0;
    int                       m_lastWaveType  = 0;
    qint64                    m_lastScanMs    = 0;
};
