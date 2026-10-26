/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyDriver.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <uskey/UsKeyDriver.h>
#include <log/Logger.h>
#include <QElapsedTimer>
#include <QMutexLocker>
#include <cstring>

// ----------------------------------------------------------------------------

UsKeyDriver::UsKeyDriver(QObject* parent)
    : QObject(parent)
{
}

UsKeyDriver::~UsKeyDriver()
{
    if (m_lib.isLoaded())
        m_lib.unload();
}

bool UsKeyDriver::load(const QString& dllPath)
{
    m_lib.setFileName(dllPath);
    if (!m_lib.load()) {
        emit evError(QString("Failed to load %1: %2").arg(dllPath, m_lib.errorString()));
        return false;
    }

    m_fn = reinterpret_cast<ApintUsb_t>(m_lib.resolve("ApintUsb"));
    if (!m_fn) {
        emit evError("ApintUsb not found in " + dllPath);
        m_lib.unload();
        return false;
    }

    call(0, "RunExeX32");

    return true;
}

bool UsKeyDriver::isLoaded() const
{
    return m_fn != nullptr;
}


long UsKeyDriver::call(int deviceNum, const char* cmd,
                       double i1, double i2, double i3,
                       double i4, double i5, double i6,
                       short* o1, short* o2, short* o3,
                       short* o4, short* o5, short* o6,
                       unsigned short* arr, long* len)
{
    QMutexLocker locker(&m_mutex);

    short          dummy[6]  = {};
    unsigned short dummyArr  = 0;
    long           dummyLen  = 0;

    const bool bTraceCall = (0 != strcmp(cmd, "A-scan"));
    if (bTraceCall)
        Logger::logInfo(QString("[UsKeyDriver] call - ENTER cmd='%1' device=%2").arg(cmd).arg(deviceNum));
    QElapsedTimer timer;
    timer.start();

    long ret = m_fn(
        1,
        static_cast<unsigned long>(deviceNum),
        const_cast<char*>(cmd),
        i1, i2, i3, i4, i5, i6,
        o1 ? o1 : &dummy[0],
        o2 ? o2 : &dummy[1],
        o3 ? o3 : &dummy[2],
        o4 ? o4 : &dummy[3],
        o5 ? o5 : &dummy[4],
        o6 ? o6 : &dummy[5],
        arr ? arr : &dummyArr,
        len ? len : &dummyLen
    );

    if (bTraceCall)
        Logger::logInfo(QString("[UsKeyDriver] call - EXIT  cmd='%1' device=%2 ret=%3 elapsed=%4 ms")
                        .arg(cmd).arg(deviceNum).arg(ret).arg(timer.elapsed()));

    return ret;
}

void UsKeyDriver::restartHelper()
{
    if (!m_fn) return;
    call(0, "KillExeX32");
    call(0, "RunExeX32");
}

// ----------------------------------------------------------------------------
// -- Initialization

bool UsKeyDriver::initUsb(int deviceNum)
{
    // LabVIEW convention: returns non-zero on success (device found), 0 if not present.
    return call(deviceNum, "Init usb") != 0;
}

bool UsKeyDriver::loadConfiguration(int deviceNum)
{
    // Same convention as "Init usb": the x64 DLL returns non-zero (1) on success
    return call(deviceNum, "load configuration", 1) != 0;
}

// ----------------------------------------------------------------------------
// -- Transmitter

bool UsKeyDriver::setPrf(int deviceNum, double kHz)
{
    return call(deviceNum, "Prf", kHz) == 0;
}

bool UsKeyDriver::setVoltage(int deviceNum, double volts)
{
    return call(deviceNum, "voltage", volts) == 0;
}

bool UsKeyDriver::setWidth(int deviceNum, double width)
{
    return call(deviceNum, "width", width) == 0;
}

bool UsKeyDriver::setPulseDelay(int deviceNum, double step)
{
    return call(deviceNum, "pulse delay", step) == 0;
}

// ----------------------------------------------------------------------------
// -- Receiver

bool UsKeyDriver::setFilterMode(int deviceNum, double filter, double mode)
{
    return call(deviceNum, "filter/mode", filter, mode) == 0;
}

bool UsKeyDriver::setWave(int deviceNum, double waveType)
{
    return call(deviceNum, "wave", waveType) == 0;
}

bool UsKeyDriver::setGain(int deviceNum, double dB)
{
    return call(deviceNum, "Gain", dB) == 0;
}

bool UsKeyDriver::setScaleDelay(int deviceNum, double us)
{
    return call(deviceNum, "scale delay", us) == 0;
}

bool UsKeyDriver::setSamplingFreqMode(int deviceNum, double freq, double mode)
{
    return call(deviceNum, "samplingfreqmode", freq, mode) == 0;
}

// ----------------------------------------------------------------------------
// -- Gates

bool UsKeyDriver::setGatePosition(int deviceNum, int gate, double us)
{
    return call(deviceNum, "gate position", gate, us) == 0;
}

bool UsKeyDriver::setGateWidth(int deviceNum, int gate, double us)
{
    return call(deviceNum, "gate width", gate, us) == 0;
}

bool UsKeyDriver::setGateHeight(int deviceNum, int gate, double percent)
{
    return call(deviceNum, "gate hight", gate, percent) == 0;
}

bool UsKeyDriver::setRelays(int deviceNum, double bitmask)
{
    return call(deviceNum, "relays", bitmask) == 0;
}

// ----------------------------------------------------------------------------
// -- Acquisition

bool UsKeyDriver::scaleAscanCounter(int deviceNum, int nbSamples)
{
    // Must be called once before the A-scan acquisition loop.
    // The DLL expects nbSamples/2 as the counter value.
    return call(deviceNum, "Scale A-scan counter",
                static_cast<double>(nbSamples / 2)) == 0;
}

bool UsKeyDriver::getAscan(int deviceNum, int dataType, int nbSamples, int waveType,
                            QVector<unsigned short>& outData)
{
    outData.resize(nbSamples);
    long len = static_cast<long>(nbSamples);

    long ret = call(
        deviceNum, "A-scan",
        static_cast<double>(dataType),
        0,
        static_cast<double>(nbSamples),
        static_cast<double>(waveType),
        0, 0,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        outData.data(), &len
    );

    outData.resize(static_cast<int>(len));
    return ret == 0;
}

bool UsKeyDriver::getMeasures(int deviceNum,
                               short* alarm1, short* amplitude1, short* distance1,
                               short* alarm2, short* amplitude2, short* distance2)
{
    long len = 0;
    return call(
        deviceNum, "measures",
        0, 0, 0, 0, 0, 0,
        alarm1, amplitude1, distance1,
        alarm2, amplitude2, distance2,
        nullptr, &len
    ) == 0;
}
