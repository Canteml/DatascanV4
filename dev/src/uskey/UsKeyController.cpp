/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyController.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <uskey/UsKeyController.h>
#include <uskey/UsKeyManager.h>
#include <uskey/UsKeyAScanAdapter.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncAScan.h>

#include <QCoreApplication>
#include <QElapsedTimer>
#include <log/Logger.h>

// ----------------------------------------------------------------------------

namespace {
    // paramDUtElement follows ProtMsg (FUNC_ASCAN_PARAM_D_UT_ELEMENT):
    // 1 = mono-element, 2 = bi-element (0 = not set yet, treated as mono).
    // The DLL "filter/mode" command expects mode 0 = T/R connected (mono),
    // 1 = T/R disconnected (bi).
    double utElementToHwMode(quint8 element)
    {
        return (2 == element) ? 1.0 : 0.0;
    }
}

// ----------------------------------------------------------------------------

UsKeyController::UsKeyController(QObject* parent)
    : QObject(parent)
    , m_manager(new UsKeyManager(this))
    , m_adapter(new UsKeyAScanAdapter(this))
{
    connect(m_manager, &UsKeyManager::evError,
            this,      &UsKeyController::onManagerError);
}

bool UsKeyController::activate(VectorStd* vectorStd)
{
    if (m_active)
        return true;

    QElapsedTimer activateTimer;
    activateTimer.start();
    Logger::logInfo("[UsKey] activate: start");

    m_vectorStd = vectorStd;

    const QString dllPath = QCoreApplication::applicationDirPath() + "/ap_int_usb";

    if (!m_manager->initialize(dllPath)) {
        Logger::logError(QString("[UsKey] activate: cannot load DLL at %1").arg(dllPath));
        emit evError(QString("US-Key: cannot load DLL at %1").arg(dllPath));
        return false;
    }
    Logger::logInfo(QString("[UsKey] activate: DLL loaded from %1").arg(dllPath));

    m_vectorStd->activateStandalone();

    m_adapter->bind(m_manager, m_vectorStd->funcAScan0());

    // On reconnection (m_active already true) re-push all hardware parameters.
    // At first connection m_active is still false, so the lambda is a no-op.
    connect(m_manager, &UsKeyManager::evConnected, this, [this]() {
        if (!m_active)
            return;
        Logger::logInfo("[UsKey] device reconnected — syncing hardware parameters");
        m_manager->setSamplingFreqMode(1.0, 0.0);
        syncToHardware();
    });

    Logger::logInfo("[UsKey] activate: connecting to device 0");
    QElapsedTimer connectTimer;
    connectTimer.start();
    bool bConnected = m_manager->connectDevice(0);
    Logger::logInfo(QString("[UsKey] activate: connectDevice(0) returned %1, elapsed=%2 ms")
                    .arg(bConnected ? "true" : "false").arg(connectTimer.elapsed()));

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDVoltageChanged,
            this, [this](quint8 voltage) {
                m_manager->setVoltage(static_cast<double>(voltage));
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDGainChanged,
            this, [this](quint8 dB) {
                m_manager->setGain(static_cast<double>(dB));
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDFilterModeChanged,
            this, [this](quint8 filter) {
                double mode = utElementToHwMode(m_vectorStd->funcAScan0()->getParamDUtElement());
                m_manager->setFilterMode(static_cast<double>(filter), mode);
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDUtElementChanged,
            this, [this](quint8 element) {
                double filter = static_cast<double>(m_vectorStd->funcAScan0()->getParamDFilterMode());
                m_manager->setFilterMode(filter, utElementToHwMode(element));
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDWaveTypeChanged,
            this, [this](quint8 waveType) {
                // waveType 3 = RF mode (software-only): keep hardware at full-rectified (0)
                m_manager->setWave(waveType < 3 ? static_cast<double>(waveType) : 0.0);
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramLProbeDelayMmChanged,
            this, [this](qreal delayMm) {
                m_manager->setScaleDelay(delayMm * 0.377);
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramLGate0StartChanged,
            this, [this](qreal startMm) {
                m_manager->setGatePosition(1, startMm * 625.0);
            });

    connect(m_vectorStd->funcAScan0(), &VectorFuncAScan::paramDPulseWidthChanged,
            this, [this](qint16 us) {
                m_manager->setWidth(us * 0.625);
            });

    // Synchronisation initiale
    m_manager->setVoltage(static_cast<double>(m_vectorStd->funcAScan0()->getParamDVoltage()));
    m_manager->setGain(static_cast<double>(m_vectorStd->funcAScan0()->getParamDGain()));
    m_manager->setWidth(static_cast<double>(m_vectorStd->funcAScan0()->getParamDPulseWidth()) / 1.6);

    // freq=1 → 80MHz, mode=0 → T/R connected
    m_manager->setSamplingFreqMode(1.0, 0.0);

    // 4000 samples, dataType=0 (HF raw), waveType=0 (full rectified)
    m_manager->startAcquisition(4000, 0, 0);

    m_active = true;
    emit evActivated();
    Logger::logInfo(QString("[UsKey] activate: end, total elapsed=%1 ms").arg(activateTimer.elapsed()));
    return true;
}

void UsKeyController::syncToHardware()
{
    if (!m_active || !m_vectorStd)
        return;

    VectorFuncAScan* ascan = m_vectorStd->funcAScan0();

    m_manager->setVoltage(static_cast<double>(ascan->getParamDVoltage()));
    m_manager->setGain(static_cast<double>(ascan->getParamDGain()));
    m_manager->setWidth(static_cast<double>(ascan->getParamDPulseWidth()) * 0.625);

    quint8 waveType = ascan->getParamDWaveType();
    m_manager->setWave(waveType < 3 ? static_cast<double>(waveType) : 0.0);

    m_manager->setFilterMode(static_cast<double>(ascan->getParamDFilterMode()),
                             utElementToHwMode(ascan->getParamDUtElement()));

    m_manager->setScaleDelay(ascan->getParamLProbeDelayMm() * 0.377);
    m_manager->setGatePosition(1, ascan->getParamLGate0Start() * 625.0);
}

void UsKeyController::deactivate()
{
    if (!m_active)
        return;

    m_adapter->unbind();
    m_manager->stopAcquisition();
    m_manager->disconnectDevice();

    m_active = false;
    emit evDeactivated();
}

void UsKeyController::onManagerError(const QString& msg)
{
    emit evError(msg);
}
