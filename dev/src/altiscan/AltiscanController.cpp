/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : AltiscanController.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 21/07/2026
#
# ==========================================================================*/

#include <altiscan/AltiscanController.h>
#include <altiscan/AltiscanSerialLink.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/vector/VectorFuncMove.h>
#include <v2/vector/VectorFuncPressureSensor.h>
#include <v2/vector/VectorFuncInertialSensor.h>
#include <log/Logger.h>

// ----------------------------------------------------------------------------

AltiscanController::AltiscanController(QObject* parent)
    : QObject(parent)
{
}

bool AltiscanController::activate(VectorStd* vectorStd, const QString& portName)
{
    if (m_active)
        return true;

    m_pVectorStd = vectorStd;

    m_pLink = new AltiscanSerialLink(this);
    connect(m_pLink, &AltiscanSerialLink::evPositionMm,  this, &AltiscanController::onPositionMm);
    connect(m_pLink, &AltiscanSerialLink::evError,       this, &AltiscanController::onLinkError);
    connect(m_pLink, &AltiscanSerialLink::evPressureRaw, this, &AltiscanController::onPressureRaw);
    connect(m_pLink, &AltiscanSerialLink::evInertialRaw, this, &AltiscanController::onInertialRaw);

    m_pLink->setOdometry(m_wheelDiameterMm, static_cast<quint32>(m_coderResolution));

    if (!m_pLink->open(portName)) {
        delete m_pLink;
        m_pLink = nullptr;
        return false;
    }

    if (m_pVectorStd->funcMove()) {
        connect(m_pVectorStd->funcMove(), &VectorFuncMove::evLocalJoyXY, this, &AltiscanController::onJoyXY);
    }

    Logger::logInfo(QString("[AltiscanController] active on %1").arg(portName));

    m_active = true;
    emit evActivated();
    return true;
}

void AltiscanController::deactivate()
{
    if (!m_active)
        return;

    if (m_pVectorStd && m_pVectorStd->funcMove()) {
        disconnect(m_pVectorStd->funcMove(), &VectorFuncMove::evLocalJoyXY, this, &AltiscanController::onJoyXY);
    }

    if (m_pLink) {
        m_pLink->close();
        delete m_pLink;
        m_pLink = nullptr;
    }

    m_pVectorStd = nullptr;
    m_active     = false;
    emit evDeactivated();
}

void AltiscanController::setOdometry(qreal wheelDiameterMm, qint32 coderResolution)
{
    m_wheelDiameterMm = wheelDiameterMm;
    m_coderResolution = coderResolution;
    if (m_pLink)
        m_pLink->setOdometry(m_wheelDiameterMm, static_cast<quint32>(m_coderResolution));
}

void AltiscanController::setEncoderReverse(bool reverse)
{
    m_encoderReverse = reverse;
}

// ----------------------------------------------------------------------------

void AltiscanController::onPositionMm(qreal mm)
{
    if (m_encoderReverse)
        mm = -mm;
    ++m_positionCount;

    if (m_positionCount == 1 || m_positionCount % 200 == 0)
        Logger::logInfo(QString("[AltiscanController] pos #%1  %2 mm").arg(m_positionCount).arg(mm, 0, 'f', 3));

    if (m_pVectorStd && m_pVectorStd->funcPosition())
        m_pVectorStd->funcPosition()->injectEncoderPositionMm(mm);
}

void AltiscanController::onLinkError(const QString& msg)
{
    emit evError(msg);
}

void AltiscanController::onJoyXY(qint8 x, qint8 y)
{
    if (m_pLink)
        m_pLink->setJoystick(x, y);
}

void AltiscanController::onPressureRaw(quint16 milliBar)
{
    // isEcho=true: only updates the local value + emits the Changed signal,
    // does not attempt to build/send a ProtMsg (no v2 ComDevice exists here).
    if (m_pVectorStd && m_pVectorStd->funcPressureSensor0())
        m_pVectorStd->funcPressureSensor0()->setParamDPressureB(milliBar, true);
}

void AltiscanController::onInertialRaw(qint16 aX, qint16 aY, qint16 aZ, qint16 /*oX*/, qint16 /*oY*/, qint16 /*oZ*/)
{
    // oX/oY/oZ (second raw triplet from the 'o' frame) have no destination
    // in the current app model yet - only the accelerometer triplet maps to
    // VectorFuncInertialSensor::paramDInertialSensor.
    if (m_pVectorStd && m_pVectorStd->funcInertialSensor())
        m_pVectorStd->funcInertialSensor()->setParamDInertialSensor(InertialSensorXYZ(aX, aY, aZ), true);
}
