/*============================================================================
#
#   Copyright Roboplanet 2026
#
# ----------------------------------------------------------------------------
#
# File name   : SerialPortRouter.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/10/2026
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>

class VectorStd;
class ComLinkManager;
class AltiscanController;
class EncoderController;

// ----------------------------------------------------------------------------
// -- SerialPortRouter
//
// Owns the "Port série de secours" setting (CentralPage1) and gives that
// serial port to the controller matching the selected product (Porteur):
//   - UT POLE           -> EncoderController  (UT POLE encoder)
//   - ALTISCAN V2 / V14 -> AltiscanController (legacy '#' protocol)
//   - other products    -> nobody
// The port is reserved in ComLinkManager so that the v2 discovery does not
// open it. If the port is missing or busy, activation is retried periodically.

class SerialPortRouter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)

public:
    SerialPortRouter(VectorStd* vectorStd,
                     ComLinkManager* linkManager,
                     AltiscanController* altiscan,
                     EncoderController* encoder,
                     QObject* parent = nullptr);

    QString portName() const { return m_portName; }
    void    setPortName(const QString& value);

    // Serial ports usable as backup port. Each entry: { "text": label, "address": "COM6" }
    Q_INVOKABLE QVariantList availablePorts() const;

    void apply();   ///< (re)assign the port according to the current product

signals:
    void portNameChanged(const QString&);

private slots:
    void onProdIdChanged(quint32 prodId);
    void onRetry();

private:
    enum Target { TARGET_NONE, TARGET_ENCODER, TARGET_ALTISCAN };

    Target targetForProduct(quint32 prodId) const;
    bool   isTargetActive() const;
    bool   tryActivate();
    void   deactivateAll();

    VectorStd*          m_vectorStd;
    ComLinkManager*     m_linkManager;
    AltiscanController* m_altiscan;
    EncoderController*  m_encoder;

    QString m_portName;
    Target  m_target = TARGET_NONE;
    QTimer  m_timRetry;

    static constexpr int RETRY_PERIOD_MS = 2000;
};
