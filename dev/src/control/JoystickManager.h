/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : JoystickManager.h
# Author      : Thierry Lechevallier
# Date        : 15/07/2020
#
# ==========================================================================*/


#pragma once

// -- Internal includes
#include "log/Loggable.h"
class vectorManager;
class ViroleManager;

// -- Qt includes
#include <QtCore/QObject>
#include <QObject>
#include <QDebug>
class QTimer;
struct _SDL_Joystick;

/**
 * @brief The JoystickManager class
 *
 * Use the first joystick found.
 * Properties :
 *      - Axes (X/Y) : qreal in [-1..1]
 *      - 1 button : bool (false / true)
 *
 * Implement a dead range (30%) for the axes.
 *
 */

class JoystickManager : public QObject, public Loggable
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ getIsConnected() NOTIFY isConnectedChanged)
public:
    static JoystickManager * makeInstance();
    static JoystickManager * getInstance();

    ~JoystickManager();

    qreal getAxeX() const;
    qreal getAxeY() const;
    bool getButtonPressed() const;

    bool start();
    void stop();

public slots:
    bool getIsConnected();
    void update();

signals:
    void evAxeX(qreal);
    void evAxeY(qreal);
    void evButtonPressed();
    void isConnectedChanged(bool);

private:
    JoystickManager();
    void setConnection(bool p_connected);
    void setAxeX(qreal);
    void setAxeY(qreal);
    void setButtonPressed(bool);
    qreal computeDeadRangeAxe(qreal);

    int m_idJoystick;
    bool m_isConnected;
    qreal m_axeX;
    qreal m_axeY;
    bool m_button;

    static const int POLL_JOYSTICK;
    static const qreal AXE_DEAD_RANDE;
    QTimer * m_timPoll;

    static JoystickManager * m_pMyInstance;
    vectorManager * m_pVectorManager;
    _SDL_Joystick * m_joy;
    ViroleManager * m_pViroleManager;
};
