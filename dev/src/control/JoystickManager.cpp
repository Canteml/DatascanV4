/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AScanGraph.h
# Author      : Thierry Lechevallier
# Date        : 05/11/2021
#
# ==========================================================================*/

#include "JoystickManager.h"
#include <QTimer>
#include <v2/exam/ExamManager.h>

#include <v2/vector/VectorFuncMove.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorManager.h>

// SDL
#include <SDL2/SDL.h>

JoystickManager *JoystickManager::m_pMyInstance = nullptr;

const int JoystickManager::POLL_JOYSTICK = 10;
const qreal JoystickManager::AXE_DEAD_RANDE = 0.1;

JoystickManager::JoystickManager() : Loggable("JoystickManager")
{
    m_idJoystick = -1;
    m_joy = nullptr;
    m_isConnected = false;
    m_axeX = 0;
    m_axeY = 0;

    m_timPoll = new QTimer();
    m_timPoll->setInterval(POLL_JOYSTICK);
    m_timPoll->setSingleShot(false);
    connect(m_timPoll, &QTimer::timeout, this, &JoystickManager::update);
}

JoystickManager::~JoystickManager()
{
    // delete timer
    m_timPoll->stop();
    delete m_timPoll;
    m_timPoll = nullptr;
    SDL_Quit();
}

bool JoystickManager::start()
{
    bool bRet = true;

    m_pFuncMove = VectorManager::getInstance()->getVectorStd()->funcMove();

    if (SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)) {
        fLogError("Fail to init SDL");
        bRet = false;
    }

    if( bRet ) {
        m_timPoll->start();
    }

    return bRet;
}

void JoystickManager::stop()
{
    m_timPoll->stop();
}

void JoystickManager::setConnection(bool p_connected)
{
    if( !p_connected ) {
        m_idJoystick = -1;
        m_pFuncMove->stopMove();
    }
    if( p_connected != m_isConnected ) {
        m_isConnected = p_connected;
        emit isConnectedChanged(m_isConnected);
    }
}

void JoystickManager::setAxeX(qreal p_value)
{
    if( m_axeX != p_value ) {
        m_axeX = p_value;
    }
}

void JoystickManager::setAxeY(qreal p_value)
{
    if( m_axeY != p_value ) {
        m_axeY = p_value;
    }
}

qreal JoystickManager::computeDeadRangeAxe(qreal p_value)
{
    qreal r = 0;
    if(p_value>0) {
        if( p_value>AXE_DEAD_RANDE ) {
            r = (p_value-AXE_DEAD_RANDE)*(1/(1-AXE_DEAD_RANDE));
        }
    }else {
        if( p_value < -AXE_DEAD_RANDE ) {
            r = (p_value+AXE_DEAD_RANDE)*(1/(1-AXE_DEAD_RANDE));
        }
    }
    return r;
}

JoystickManager *JoystickManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("JoystickManager::makeInstance instance already exists !");
    } else {
        m_pMyInstance = new JoystickManager();
    }

    return m_pMyInstance;
}

JoystickManager * JoystickManager::getInstance()
{
    return m_pMyInstance;
}

bool JoystickManager::getIsConnected()
{
    return  m_isConnected;
}
void JoystickManager::update()
{
    SDL_Event event;

    while (SDL_PollEvent (&event)) {
        switch (event.type) {
            case SDL_JOYDEVICEADDED:
                if( -1 == m_idJoystick ) {
                    m_joy = SDL_JoystickOpen (event.jdevice.which);
                    m_idJoystick = SDL_JoystickInstanceID(m_joy);
                    setConnection(true);
                }

                break;

            case SDL_JOYDEVICEREMOVED:
                if( event.jdevice.which == m_idJoystick ) {
                    SDL_JoystickClose (m_joy);
                    setConnection(false);
                }
                break;

            case SDL_JOYAXISMOTION:
                if( event.jdevice.which == m_idJoystick ) {
                    if( event.jaxis.axis == 0 ) {
                        // X : Left-right movement code goes here
                        qreal x = static_cast<qreal>(event.jaxis.value) / 32767;
                        x = computeDeadRangeAxe(x);
                        if( x != m_axeX ) {
                            setAxeX(x);
                        }
                    } else if( event.jaxis.axis == 1 ) {
                        // Y : Up-Down movement code goes here
                        qreal y = - static_cast<qreal>(event.jaxis.value) / 32767; // position UP is <0
                        y = computeDeadRangeAxe(y);
                        if( y != m_axeY ) {
                            setAxeY(y);
                        }
                   }
                   m_pFuncMove->setJoyXY(m_axeX*127, m_axeY*127);
                }
                break;

            case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
                if( event.jdevice.which == m_idJoystick ) {
                    if ( event.jbutton.button == 0 ) {
                        ExamManager::getInstance()->zoneAddToCurrentPosition();
                    }
                }
                break;

            default:
                fLogWarning("Joystick event not handeld : " + QString::number(event.type));
                break;
        }
    }
}
