/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : AltiscanController.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 21/07/2026
#
# ==========================================================================*/

#pragma once

#include <QObject>

class AltiscanSerialLink;
class VectorStd;

// ----------------------------------------------------------------------------
// -- AltiscanController
//
// Application-level owner of the Altiscan legacy serial link (mirrors
// EncoderController for the UT-POLE encoder). Call activate() with a
// VectorStd pointer to open the link on COM6 and start forwarding position
// updates to VectorFuncPosition::injectEncoderPositionMm() - bypassing the
// v2/ProtMsg layer entirely, since the physical robot speaks the legacy
// '#'-framed protocol (confirmed empirically, see Phase 1 hardware probe).
//
// Also subscribes to VectorFuncMove::evLocalJoyXY (fired from setJoyXY() on
// every real change, regardless of any v2/ComDevice being connected) and
// forwards the raw x/y to AltiscanSerialLink::setJoystick(), which builds
// the legacy 'v' frame - so both the physical joystick (JoystickManager)
// and the on-screen movement controls, which both ultimately call
// VectorFuncMove::setJoyXY()/moveKeyUpdate(), drive the robot without any
// change to their own code.
//
// Also forwards AltiscanSerialLink::evPressureRaw/evInertialRaw to
// VectorFuncPressureSensor0::setParamDPressureB() and
// VectorFuncInertialSensor::setParamDInertialSensor(), both called with
// isEcho=true so the generic FUNC_PARAM setter only updates the local value
// and notifies QML - it does not attempt to serialize/send a ProtMsg, which
// would require a v2 ComDevice that does not exist on this transport (same
// reasoning as the position bypass, verified against
// IntComMsg::isMpc()/ComDeviceManager::sendMsg() before relying on it).
//
// deactivate() closes the link cleanly.

class AltiscanController : public QObject
{
    Q_OBJECT

public:
    explicit AltiscanController(QObject* parent = nullptr);

    bool activate(VectorStd* vectorStd, const QString& portName = "COM6");
    void deactivate();

    bool isActive() const { return m_active; }

signals:
    void evError(const QString& msg);
    void evActivated();
    void evDeactivated();

private slots:
    void onPositionMm(qreal mm);
    void onLinkError(const QString& msg);
    void onJoyXY(qint8 x, qint8 y);
    void onPressureRaw(quint16 milliBar);
    void onInertialRaw(qint16 aX, qint16 aY, qint16 aZ, qint16 oX, qint16 oY, qint16 oZ);

private:
    AltiscanSerialLink* m_pLink         = nullptr;
    VectorStd*          m_pVectorStd    = nullptr;
    bool                m_active        = false;
    quint32             m_positionCount = 0;
};
