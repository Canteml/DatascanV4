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
#include <cstdlib>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <dbt.h>
#endif

// ----------------------------------------------------------------------------
// -- UsKeyAcquisitionThread (private, defined only in this translation unit)
//
// Runs a tight loop calling ApintUsb("Ascan") and emitting evNewRawAScan.
// Stopped by requestStop() + wait().

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

signals:
    void evNewRawAScan(QVector<unsigned short> data, qint64 timestampMs);
    void evDataLost();

protected:
    void run() override
    {
        // Paced at ~100 Hz (10 ms period), matching the legacy gestionuskey.cpp
        // acquisition loop (ACQUIRE_PERIODE = 10 ms). This is not just about
        // acquisition rate: UsKeyDriver::call() now serializes every call
        // into the vendor DLL with a QMutex (required - the DLL is called
        // from this thread, the GUI thread, and UsKeyManager's connect-thread,
        // and is not known to be safe under concurrent calls). Without a gap
        // between iterations, this thread reacquires the mutex almost
        // continuously, and a GUI-thread call arriving between two iterations
        // can be starved for an unpredictable time - since Qt Quick rendering
        // and all other GUI-thread slots run on the same single event loop,
        // that stall was observed to blank the A-scan graph entirely (not
        // just the intermittent flicker this pacing+mutex pair is meant to
        // fix). The sleep guarantees the mutex is released for a real gap
        // every cycle.
        static constexpr qint64 TARGET_PERIOD_US = 10000; // 10 ms = 100 Hz

        QVector<unsigned short> buf;
        bool hadData = false;
        QElapsedTimer loopTimer;
        while (!m_stop) {
            loopTimer.start();

            // Return code not checked: x64 DLL returns non-zero even on success.
            // Data validity is determined by buf.size() > 0 after the call.
            m_driver->getAscan(m_deviceNum, m_dataType, m_nbSamples, m_waveType, buf);

            if (!buf.isEmpty()) {
                if (!hadData) {
                    hadData = true;
                }
                emit evNewRawAScan(buf, QDateTime::currentMSecsSinceEpoch());
            } else if (hadData) {
                hadData = false;
                emit evDataLost();
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
    int               m_dataType  = 0;
    volatile int      m_waveType  = 0;
    volatile bool     m_stop      = false;
};

// Resolved through MOC_DIR, which qmake already adds to the include path.
#include "UsKeyManager.moc"

// ----------------------------------------------------------------------------
// -- UsbDeviceFilter
//
// Separate non-QObject class so MOC does not generate a conflicting
// nativeEventFilter implementation for UsKeyManager itself.

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
        // DBT_DEVNODES_CHANGED fires on any USB change (add or remove).
        // Delay 200 ms so the current getAscan() DLL call can complete,
        // then probe the device: if initUsb() fails, it truly disconnected.
        // The probe itself runs on a background thread (see probeStillPresent) —
        // calling m_driver->initUsb() directly here would block the GUI thread.
        // m_mgr is used as context so the lambda is cancelled if UsKeyManager is destroyed.
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
    , m_driver(new UsKeyDriver())   // no QObject parent: moved to m_connectThread below —
                                    // a parented QObject cannot be moved across threads
{
    connect(m_driver, &UsKeyDriver::evError, this, &UsKeyManager::onDriverError);
    connect(this, &UsKeyManager::evNewRawAScan, this,
            [this](QVector<unsigned short>, qint64 ts) {
                m_lastScanMs = ts;
            });

    // m_driver lives here for the rest of its life: every initUsb()/
    // loadConfiguration() call (startConnectAttempt/probeStillPresent) is
    // posted to this single, persistent thread instead of a fresh one per
    // attempt — see m_connectThread's declaration for why.
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

    // quit() lets the connect-thread's event loop finish; wait() blocks until
    // any in-flight initUsb()/loadConfiguration() call actually returns.
    // That call can legitimately take several seconds (observed up to 30+ s
    // on real hardware) or, rarely, appear to never return at all. An
    // unbounded wait() here previously meant: if the user closed the app
    // while such a call was in flight, this destructor - and therefore all
    // of main()'s teardown, since CoreApp is a stack object destroyed after
    // execUiQml() returns - would hang forever, only recoverable via a
    // forced kill from Task Manager (which orphans the ExeUSB2X86.exe
    // helper, since it skips delete m_driver below).
    //
    // A first attempt at fixing this called QThread::terminate() on timeout
    // and then continued running the rest of the process normally. That is
    // NOT safe here: terminate() rips the OS thread out from under the
    // vendor DLL mid-call, and that DLL is built on the LabVIEW runtime
    // (lvrt.dll), which - once loaded - appears to hook into this process's
    // Winsock calls (a minidump showed a later access violation *inside*
    // lvrt.dll, reached via Qt6Network -> WS2_32/mswsock, triggered by
    // unrelated socket cleanup elsewhere in the app). Continuing to run
    // arbitrary code afterward risks touching whatever terminate() left in
    // an inconsistent state. Since the job object set up in main() already
    // guarantees ExeUSB2X86.exe is killed when THIS process ends for any
    // reason, the only genuinely safe response to a stuck DLL call is to
    // end the process immediately here rather than try to keep going.
    m_connectThread->quit();
    if (!m_connectThread->wait(5000)) {
        Logger::logError("[UsKeyManager] connect-thread did not stop within 5s (DLL call stuck in the "
                         "vendor runtime) - ending the process now instead of forcibly interrupting it; "
                         "the job object set up in main() will clean up the ExeUSB2X86.exe helper.");
        std::_Exit(1);
    }

    // m_driver has no QObject parent (see constructor), so it is not deleted
    // by the usual child-cascade — delete it explicitly now that its thread
    // has fully and cleanly stopped.
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
    return true; // connection attempt started asynchronously; see onConnectAttemptFinished()
}

// Posts the actual (blocking) vendor-DLL probe onto m_connectThread — the
// same persistent thread for every attempt — so the caller's thread (always
// the GUI thread in this app) never waits on it.
void UsKeyManager::startConnectAttempt()
{
    if (m_connecting)
        return; // an attempt is already in flight — the retry timer will tick again later
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
        startRetryTimer();
        return;
    }

    if (m_retryTimer)
        m_retryTimer->stop();

    if (!configOk)
        Logger::logWarning(QString("[UsKey] device %1 connected but loadConfiguration failed").arg(m_deviceNum));

    m_connected = true;
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
    m_connected = false;
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

    // x64 DLL: return value not checked (LabVIEW convention, MATLAB reference never checks it)
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

void UsKeyManager::stopAcquisition()
{
    if (!m_thread)
        return;

    if (m_watchdogTimer)
        m_watchdogTimer->stop();

    m_thread->requestStop();
    m_thread->wait();
    m_thread = nullptr;
}

bool UsKeyManager::isAcquiring() const
{
    return m_thread && m_thread->isRunning();
}

// ----------------------------------------------------------------------------
// -- Transmitter parameters

void UsKeyManager::setPrf(double kHz)
{
    if (m_connected) m_driver->setPrf(m_deviceNum, kHz);
}

void UsKeyManager::setVoltage(double volts)
{
    if (m_connected) m_driver->setVoltage(m_deviceNum, volts);
}

void UsKeyManager::setWidth(double width)
{
    if (m_connected) m_driver->setWidth(m_deviceNum, width);
}

void UsKeyManager::setPulseDelay(double step)
{
    if (m_connected) m_driver->setPulseDelay(m_deviceNum, step);
}

// ----------------------------------------------------------------------------
// -- Receiver parameters

void UsKeyManager::setFilterMode(double filter, double mode)
{
    if (m_connected) m_driver->setFilterMode(m_deviceNum, filter, mode);
}

void UsKeyManager::setWave(double waveType)
{
    if (m_connected) {
        m_driver->setWave(m_deviceNum, waveType);
        if (m_thread)
            m_thread->setWaveType(static_cast<int>(waveType));
    }
}

void UsKeyManager::setGain(double dB)
{
    if (m_connected) m_driver->setGain(m_deviceNum, dB);
}

void UsKeyManager::setScaleDelay(double us)
{
    if (m_connected) m_driver->setScaleDelay(m_deviceNum, us);
}

void UsKeyManager::setSamplingFreqMode(double freq, double mode)
{
    if (m_connected) m_driver->setSamplingFreqMode(m_deviceNum, freq, mode);
}

// ----------------------------------------------------------------------------
// -- Gate parameters

void UsKeyManager::setGatePosition(int gate, double us)
{
    if (m_connected) m_driver->setGatePosition(m_deviceNum, gate, us);
}

void UsKeyManager::setGateWidth(int gate, double us)
{
    if (m_connected) m_driver->setGateWidth(m_deviceNum, gate, us);
}

void UsKeyManager::setGateHeight(int gate, double percent)
{
    if (m_connected) m_driver->setGateHeight(m_deviceNum, gate, percent);
}

void UsKeyManager::setRelays(double bitmask)
{
    if (m_connected) m_driver->setRelays(m_deviceNum, bitmask);
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
    Logger::logError(QString("[UsKey] device %1 disconnected — retrying every 2 s").arg(m_deviceNum));
    stopAcquisition();
    startRetryTimer();
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
    // Was: called m_driver->initUsb()/loadConfiguration() directly on this
    // GUI-thread timer tick, blocking the UI for as long as the vendor DLL
    // took (observed 12+ s, sometimes indefinitely). Now delegates to the
    // same background-thread probe used by the initial connectDevice().
    startConnectAttempt();
}
