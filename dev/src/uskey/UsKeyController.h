/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyController.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#pragma once

#include <QObject>

class UsKeyManager;
class UsKeyAScanAdapter;
class VectorStd;

// ----------------------------------------------------------------------------
// -- UsKeyController
//
// Application-level owner of the US-Key standalone pipeline.
// Call activate() once the QML AScanGraph is connected; it will:
//   1. Load ap_int_usb.dll
//   2. Call VectorStd::activateStandalone()  (enables ASCAN0 + POSITION)
//   3. Bind UsKeyAScanAdapter → VectorFuncAScan0
//   4. Connect to the device and start the acquisition thread
//
// The US-Key must be connected before the application starts.
// deactivate() stops acquisition cleanly.

class UsKeyController : public QObject
{
    Q_OBJECT

public:
    explicit UsKeyController(QObject* parent = nullptr);

    bool activate(VectorStd* vectorStd);
    void deactivate();
    void syncToHardware();

    bool isActive() const { return m_active; }

signals:
    void evError(const QString& msg);
    void evActivated();
    void evDeactivated();

private slots:
    void onManagerError(const QString& msg);

private:
    UsKeyManager*      m_manager   = nullptr;
    UsKeyAScanAdapter* m_adapter   = nullptr;
    VectorStd*         m_vectorStd = nullptr;
    bool               m_active    = false;
};
