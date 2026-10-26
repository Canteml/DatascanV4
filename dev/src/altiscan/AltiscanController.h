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

class AltiscanController : public QObject
{
    Q_OBJECT

public:
    explicit AltiscanController(QObject* parent = nullptr);

    bool activate(VectorStd* vectorStd, const QString& portName = "COM6");
    void deactivate();

    bool isActive() const { return m_active; }

public slots:
    void setOdometry(qreal wheelDiameterMm, qint32 coderResolution);
    void setEncoderReverse(bool reverse);

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
    AltiscanSerialLink* m_pLink           = nullptr;
    VectorStd*          m_pVectorStd      = nullptr;
    bool                m_active          = false;
    quint32             m_positionCount   = 0;
    qreal               m_wheelDiameterMm = 52.5;
    qint32              m_coderResolution = 1024;
    bool                m_encoderReverse  = false;
};
