/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyDriver.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QLibrary>
#include <QVector>
#include <QMutex>

// ----------------------------------------------------------------------------
// -- UsKeyDriver
//
// Wrapper around ap_int_usb.dll (Lecoeur Electronique).
// Loads the DLL at runtime via QLibrary and exposes a typed C++ API.
//
// All methods return true on success (ApintUsb returned 0).
// On error, evError is emitted with a description.
//
// Thread-safety: every public method funnels through the private call()
// helper, which serializes access to ApintUsb with an internal QMutex. This
// is REQUIRED, not defensive: this object is called from three different
// threads in practice (GUI thread for parameter setters, the persistent
// UsKeyManager connect-thread for initUsb()/loadConfiguration(), and the
// acquisition thread's tight getAscan() loop, which has no Qt event loop of
// its own — see UsKeyManager.h's m_connectThread comment for why a
// QMetaObject::invokeMethod-based approach cannot serialize against that
// loop). The vendor DLL is built on the LabVIEW runtime (lvrt.dll) and is
// not known to be safe under concurrent calls from multiple OS threads.
//
// A-scan data is returned as QVector<unsigned short> (12-bit values in 16-bit words).
// Caller is responsible for converting to 8-bit if needed (value >> 4).
//
// Calling convention of ApintUsb: __cdecl
// DLL dependencies (must be in same directory as the exe, x64 version):
//   Ap_int_usb.dll, USB2.dll, USB2X64.dll, ExeUSB2X86.exe, Ap_int_usb.ini
//
// IMPORTANT — x64 startup sequence:
//   1. load()           → resolves ApintUsb + calls RunExeX32 (spawns ExeUSB2X86.exe)
//   2. initUsb()        → Init usb
//   3. loadConfiguration()
//   4. scaleAscanCounter()  → once, before the acquisition loop
//   5. getAscan() loop
//   6. ~UsKeyDriver()   → calls KillExeX32, then unloads DLL

class UsKeyDriver : public QObject
{
    Q_OBJECT

public:
    explicit UsKeyDriver(QObject* parent = nullptr);
    ~UsKeyDriver();

    bool load(const QString& dllPath = "ap_int_usb");
    bool isLoaded() const;


    // -- Initialization
    bool initUsb(int deviceNum);
    bool loadConfiguration(int deviceNum);

    // -- Transmitter
    bool setPrf(int deviceNum, double kHz);
    bool setVoltage(int deviceNum, double volts);         // 30–230 V
    bool setWidth(int deviceNum, double width);           // 0–255
    bool setPulseDelay(int deviceNum, double step);       // step × 1.6 µs

    // -- Receiver
    bool setFilterMode(int deviceNum, double filter, double mode);
    // filter: 0=1.25MHz 1=2.5MHz 2=5MHz 3=10MHz 4=BroadBand
    // mode:   0=T/R connected  1=T/R disconnected
    bool setWave(int deviceNum, double waveType);
    // waveType: 0=redressée  1=négative  2=positive
    bool setGain(int deviceNum, double dB);               // 0–79.9 dB
    bool setScaleDelay(int deviceNum, double us);
    bool setSamplingFreqMode(int deviceNum, double freq, double mode);
    // freq: 0=160MHz 1=80MHz 2=40MHz 3=20MHz

    // -- Gates (gate = 1, 2 or 3)
    bool setGatePosition(int deviceNum, int gate, double us);
    bool setGateWidth(int deviceNum, int gate, double us);
    bool setGateHeight(int deviceNum, int gate, double percent);
    bool setRelays(int deviceNum, double bitmask);

    // -- Acquisition
    // Must be called once with the target nbSamples before starting the A-scan loop.
    bool scaleAscanCounter(int deviceNum, int nbSamples);

    // dataType: 0=HF  1=A-scan
    // waveType: 0=full rectified  1=negative  2=positive
    bool getAscan(int deviceNum, int dataType, int nbSamples, int waveType,
                  QVector<unsigned short>& outData);

signals:
    void evError(const QString& msg);

private:
    // Signature of ApintUsb from ap_int_usb.h (calling convention __cdecl)
    typedef long (__cdecl *ApintUsb_t)(
        unsigned long productNumber,
        unsigned long channelNumber,
        char*         function,
        double in1, double in2, double in3, double in4, double in5, double in6,
        short* out1, short* out2, short* out3, short* out4, short* out5, short* out6,
        unsigned short* array,
        long*           len
    );

    long call(int deviceNum, const char* cmd,
              double i1 = 0, double i2 = 0, double i3 = 0,
              double i4 = 0, double i5 = 0, double i6 = 0,
              short* o1 = nullptr, short* o2 = nullptr, short* o3 = nullptr,
              short* o4 = nullptr, short* o5 = nullptr, short* o6 = nullptr,
              unsigned short* arr = nullptr, long* len = nullptr);

    QLibrary   m_lib;
    ApintUsb_t m_fn = nullptr;
    QMutex     m_mutex; ///< serializes every call() - see class doc above
};
