/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyManager.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <uskey/UsKeyManager.h>
#include <uskey/UsKeyDriver.h>
#include <log/Logger.h>

#include <QThread>
#include <QDateTime>
#include <QElapsedTimer>
#include <QTimer>
#include <QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <QMutex>
#include <QMutexLocker>
#include <QPair>
#include <cstdlib>
#include <functional>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <dbt.h>
#endif

// ----------------------------------------------------------------------------

class UsKeyAcquisitionThread : public QThread
{
    Q_OBJECT

public:
    void configure(UsKeyDriver* driver, int deviceNum,
                   int nbSamples, int dataType, int waveType)
    {
        m_driver    = driver;
        m_deviceNum = deviceNum;
        m_nbSamples = nbSamples;
        m_dataType  = dataType;
        m_waveType  = waveType;
    }

    void requestStop()            { m_stop = true; }
    void setWaveType(int waveType){ m_waveType = waveType; }
    void setDataType(int dataType){ m_dataType = dataType; }

    void postCommand(const QString& key, std::function<void()> fn)
    {
        QMutexLocker locker(&m_cmdMutex);
        for (auto& cmd : m_cmds) {
            if (cmd.first == key) {
                cmd.second = std::move(fn);
                return;
            }
        }
        m_cmds.append({key, std::move(fn)});
    }

signals:
    void evNewRawAScan(QVector<unsigned short> data, qint64 timestampMs);
    void evDataLost();

protected:
    void run() override
    {

        static constexpr qint64 TARGET_PERIOD_US = 10000;

        QVector<unsigned short> buf;
        bool hadData = false;
        QElapsedTimer loopTimer;
        while (!m_stop) {
            loopTimer.start();

            QVector<QPair<QString, std::function<void()>>> cmds;
            {
                QMutexLocker locker(&m_cmdMutex);
                cmds.swap(m_cmds);
            }
            for (const auto& cmd : cmds)
                cmd.second();

            m_driver->getAscan(m_deviceNum, m_dataType, m_nbSamples, m_waveType, buf);

            if (!buf.isEmpty()) {
                hadData = true;
                emit evNewRawAScan(buf, QDateTime::currentMSecsSinceEpoch());
            } else if (hadData) {
                Logger::logWarning("[UsKey] empty A-scan : device stuck, requesting an immediate restart");
                hadData = false;
                emit evDataLost();
                break;
            }

            const qint64 elapsedUs = loopTimer.nsecsElapsed() / 1000;
            if (elapsedUs < TARGET_PERIOD_US) {
                QThread::usleep(static_cast<unsigned long>(TARGET_PERIOD_US - elapsedUs));
            }
        }
    }

private:
    UsKeyDriver*      m_driver    = nullptr;
    int               m_deviceNum = 0;
    int               m_nbSamples = 4000;
    volatile int      m_dataType  = 0;
    volatile int      m_waveType  = 0;
    volatile bool     m_stop      = false;
    QMutex            m_cmdMutex;
    QVector<QPair<QString, std::function<void()>>> m_cmds;
};

#include "UsKeyManager.moc"

// ----------------------------------------------------------------------------
// -- UsbDeviceFilter

#ifdef Q_OS_WIN
class UsbDeviceFilter : public QAbstractNativeEventFilter
{
public:
    explicit UsbDeviceFilter(UsKeyManager* mgr) : m_mgr(mgr) {}

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr*) override
    {
        if (eventType != "windows_generic_MSG")
            return false;
        const auto* msg = static_cast<const MSG*>(message);
        if (msg->message != WM_DEVICECHANGE || msg->wParam != DBT_DEVNODES_CHANGED)
            return false;
        if (!m_mgr->m_connected)
            return false;
        QTimer::singleShot(200, m_mgr, [mgr = m_mgr]() {
            if (mgr->m_connected)
                mgr->probeStillPresent();
        });
        return false;
    }

private:
    UsKeyManager* m_mgr;
};
#endif

// ----------------------------------------------------------------------------
// -- UsKeyManager

UsKeyManager::UsKeyManager(QObject* parent)
    : QObject(parent)
    , m_driver(new UsKeyDriver())
{
    connect(m_driver, &UsKeyDriver::evError, this, &UsKeyManager::onDriverError);
    connect(this, &UsKeyManager::evNewRawAScan, this,
            [this](QVector<unsigned short>, qint64 ts) {
                m_lastScanMs = ts;
            });

    m_connectThread = new QThread(this);
    m_driver->moveToThread(m_connectThread);
    m_connectThread->start();

#ifdef Q_OS_WIN
    m_nativeFilter = new UsbDeviceFilter(this);
    qApp->installNativeEventFilter(m_nativeFilter);
#endif
}

UsKeyManager::~UsKeyManager()
{
#ifdef Q_OS_WIN
    qApp->removeNativeEventFilter(m_nativeFilter);
    delete m_nativeFilter;
#endif
    stopAcquisition();
    disconnectDevice();

    m_connectThread->quit();
    if (!m_connectThread->wait(5000)) {
        Logger::logError("[UsKeyManager] connect-thread did not stop within 5s (DLL call stuck in the "
                         "vendor runtime) - ending the process now instead of forcibly interrupting it; "
                         "the job object set up in main() will clean up the ExeUSB2X86.exe helper.");
        std::_Exit(1);
    }

    delete m_driver;
}

// ----------------------------------------------------------------------------
// -- Lifecycle

bool UsKeyManager::initialize(const QString& dllPath)
{
    return m_driver->load(dllPath);
}

bool UsKeyManager::connectDevice(int deviceNum)
{
    if (!m_driver->isLoaded()) {
        emit evError("DLL not loaded — call initialize() first");
        return false;
    }

    m_deviceNum = deviceNum;
    startConnectAttempt();
    return true;
}

void UsKeyManager::startConnectAttempt()
{
    if (m_connecting)
        return;
    m_connecting = true;

    UsKeyDriver* driver = m_driver;
    int deviceNum = m_deviceNum;
    Logger::logInfo(QString("[UsKey] startConnectAttempt: probing device %1 on connect thread").arg(deviceNum));

    QMetaObject::invokeMethod(m_driver, [this, driver, deviceNum]() {
        bool found = driver->initUsb(deviceNum);
        bool configOk = found && driver->loadConfiguration(deviceNum);
        QMetaObject::invokeMethod(this, [this, found, configOk]() {
            onConnectAttemptFinished(found, configOk);
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void UsKeyManager::onConnectAttemptFinished(bool found, bool configOk)
{
    m_connecting = false;

    if (!found) {
        Logger::logError(QString("[UsKey] device %1 not found or not connected — retrying every 2 s").arg(m_deviceNum));
        if (!m_absentReported) {
            m_absentReported = true;
            emit evDisconnected();
        }
        startRetryTimer();
        return;
    }

    if (m_retryTimer)
        m_retryTimer->stop();

    if (!configOk)
        Logger::logWarning(QString("[UsKey] device %1 connected but loadConfiguration failed").arg(m_deviceNum));

    m_connected      = true;
    m_absentReported = false;
    Logger::logInfo(QString("[UsKey] device %1 connected").arg(m_deviceNum));
    emit evConnected();
    startAcquisition(m_lastNbSamples, m_lastDataType, m_lastWaveType);
}

void UsKeyManager::probeStillPresent()
{
    UsKeyDriver* driver = m_driver;
    int deviceNum = m_deviceNum;

    QMetaObject::invokeMethod(m_driver, [this, driver, deviceNum]() {
        bool present = driver->initUsb(deviceNum);
        QMetaObject::invokeMethod(this, [this, present]() {
            if (!present)
                onDataLost();
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void UsKeyManager::disconnectDevice()
{
    if (!m_connected) {
        return;
    }
    stopAcquisition();
    m_connected      = false;
    m_absentReported = true;
    emit evDisconnected();
}

bool UsKeyManager::isConnected() const
{
    return m_connected;
}

int UsKeyManager::deviceNum() const
{
    return m_deviceNum;
}

// ----------------------------------------------------------------------------
// -- Acquisition

void UsKeyManager::startAcquisition(int nbSamples, int dataType, int waveType)
{
    m_lastNbSamples = nbSamples;
    m_lastDataType  = dataType;
    m_lastWaveType  = waveType;

    if (!m_connected || m_thread) {
        return;
    }

    m_driver->scaleAscanCounter(m_deviceNum, nbSamples);

    m_thread = new UsKeyAcquisitionThread();
    m_thread->configure(m_driver, m_deviceNum, nbSamples, dataType, waveType);

    connect(m_thread, &UsKeyAcquisitionThread::evNewRawAScan,
            this,     &UsKeyManager::evNewRawAScan);
    connect(m_thread, &UsKeyAcquisitionThread::evDataLost,
            this,     &UsKeyManager::onDataLost);
    connect(m_thread, &UsKeyAcquisitionThread::finished,
            m_thread, &QObject::deleteLater);

    m_lastScanMs = QDateTime::currentMSecsSinceEpoch();
    m_thread->start();

    if (!m_watchdogTimer) {
        m_watchdogTimer = new QTimer(this);
        m_watchdogTimer->setInterval(1000);
        connect(m_watchdogTimer, &QTimer::timeout, this, &UsKeyManager::onWatchdogTimeout);
    }
    m_watchdogTimer->start();
}

void UsKeyManager::stopAcquisition(bool waitForThread)
{
    if (!m_thread)
        return;

    if (m_watchdogTimer)
        m_watchdogTimer->stop();

    m_thread->requestStop();
    if (waitForThread) {
        m_thread->wait();
    } else {
        disconnect(m_thread, &UsKeyAcquisitionThread::evNewRawAScan, this, &UsKeyManager::evNewRawAScan);
        disconnect(m_thread, &UsKeyAcquisitionThread::evDataLost,    this, &UsKeyManager::onDataLost);
    }
    m_thread = nullptr;
}

bool UsKeyManager::isAcquiring() const
{
    return m_thread && m_thread->isRunning();
}

// ----------------------------------------------------------------------------
// -- Parameter writes

void UsKeyManager::postParam(const QString& key, std::function<void()> fn)
{
    if (!m_connected)
        return;
    if (m_thread)
        m_thread->postCommand(key, std::move(fn));
    else
        fn();
}

// ----------------------------------------------------------------------------
// -- Transmitter parameters

void UsKeyManager::setPrf(double kHz)
{
    postParam("Prf", [d = m_driver, n = m_deviceNum, kHz]() { d->setPrf(n, kHz); });
}

void UsKeyManager::setVoltage(double volts)
{
    postParam("voltage", [d = m_driver, n = m_deviceNum, volts]() { d->setVoltage(n, volts); });
}

void UsKeyManager::setWidth(double width)
{
    postParam("width", [d = m_driver, n = m_deviceNum, width]() { d->setWidth(n, width); });
}

void UsKeyManager::setPulseDelay(double step)
{
    postParam("pulse delay", [d = m_driver, n = m_deviceNum, step]() { d->setPulseDelay(n, step); });
}

// ----------------------------------------------------------------------------
// -- Receiver parameters

void UsKeyManager::setFilterMode(double filter, double mode)
{
    postParam("filter/mode", [d = m_driver, n = m_deviceNum, filter, mode]() { d->setFilterMode(n, filter, mode); });
}

void UsKeyManager::setWave(double waveType)
{
    const int wt = static_cast<int>(waveType);
    const bool isRF = (wt == 3);
    const int hwWave = isRF ? 0 : wt;
    const int dataType = isRF ? 0 : 1;

    m_lastWaveType = hwWave;
    m_lastDataType = dataType;

    if (!m_connected)
        return;
    postParam("wave", [d = m_driver, n = m_deviceNum, hwWave]() { d->setWave(n, hwWave); });
    if (m_thread){
        m_thread->setWaveType(hwWave);
        m_thread->setDataType(dataType);
    }
}

void UsKeyManager::setGain(double dB)
{
    postParam("Gain", [d = m_driver, n = m_deviceNum, dB]() { d->setGain(n, dB); });
}

void UsKeyManager::setScaleDelay(double us)
{
    postParam("scale delay", [d = m_driver, n = m_deviceNum, us]() { d->setScaleDelay(n, us); });
}

void UsKeyManager::setSamplingFreqMode(double freq, double mode)
{
    postParam("samplingfreqmode", [d = m_driver, n = m_deviceNum, freq, mode]() { d->setSamplingFreqMode(n, freq, mode); });
}

// ----------------------------------------------------------------------------
// -- Gate parameters

void UsKeyManager::setGatePosition(int gate, double us)
{
    postParam(QString("gate position %1").arg(gate),
              [d = m_driver, n = m_deviceNum, gate, us]() { d->setGatePosition(n, gate, us); });
}

void UsKeyManager::setGateWidth(int gate, double us)
{
    postParam(QString("gate width %1").arg(gate),
              [d = m_driver, n = m_deviceNum, gate, us]() { d->setGateWidth(n, gate, us); });
}

void UsKeyManager::setGateHeight(int gate, double percent)
{
    postParam(QString("gate hight %1").arg(gate),
              [d = m_driver, n = m_deviceNum, gate, percent]() { d->setGateHeight(n, gate, percent); });
}

void UsKeyManager::setRelays(double bitmask)
{
    postParam("relays", [d = m_driver, n = m_deviceNum, bitmask]() { d->setRelays(n, bitmask); });
}

// ----------------------------------------------------------------------------

void UsKeyManager::onDriverError(const QString& msg)
{
    emit evError(msg);
}


void UsKeyManager::onDataLost()
{
    if (!m_connected)
        return;
    m_connected = false;
    Logger::logError(QString("[UsKey] device %1 stuck or disconnected — restarting it now").arg(m_deviceNum));
    stopAcquisition(false);
    startConnectAttempt();
}

void UsKeyManager::onWatchdogTimeout()
{
    if (!m_connected || !m_thread)
        return;
    if (QDateTime::currentMSecsSinceEpoch() - m_lastScanMs > 3000)
        onDataLost();
}

void UsKeyManager::startRetryTimer()
{
    if (!m_retryTimer) {
        m_retryTimer = new QTimer(this);
        m_retryTimer->setInterval(2000);
        connect(m_retryTimer, &QTimer::timeout, this, &UsKeyManager::onRetryTimeout);
    }
    if (!m_retryTimer->isActive())
        m_retryTimer->start();
}

void UsKeyManager::onRetryTimeout()
{
    startConnectAttempt();
}
