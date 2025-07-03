/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : TestCore.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

#include <core/Core.h>
#include <QTimer>

class TestCore : public QObject, public Core
{
    Q_OBJECT
public:
    TestCore();
    ~TestCore();

    // -- Core
    QString getVersion() const override;
    QString getApplicationName() const override;

    QTimer * m_t1;
    QTimer * m_t2;

public slots :
    void onT1();    
    void onT2();

protected :
    // -- Core
    bool initApp() override;     ///< Make managers
    bool startApp() override;    ///< Start managers
    void stopApp() override;     ///< Stop managers
    void deleteApp() override;   ///< Delete managers


};
