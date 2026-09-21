/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFunc.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/prot/ProtMsg.h>
#include <xml/XmlProject.h>
#include "ComDevice.h"
class VectorStd;
class ComDevice;
class GenComLink;

Q_DECLARE_OPAQUE_POINTER(GenComLink*)
Q_DECLARE_METATYPE(GenComLink*)

// -- Qt includes
#include <qqml.h>

// ----------------------------------------------------------------------------
// -- GenVectorFunc

/**
 * @brief The GenVectorFunc class
 */
class GenVectorFunc : public QObject, public XmlProjectLoggable {
    Q_OBJECT

    Q_PROPERTY(bool isEnabled READ getIsEnabled WRITE setIsEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(ComDevice* comDevice READ getComDevice NOTIFY comDeviceChanged)
    Q_PROPERTY(bool hasReceivedMessage READ getHasReceivedMessage NOTIFY hasReceivedMessageChanged)

    QML_ELEMENT

public :
    GenVectorFunc(const QString & p_functionName, VectorStd * p_pVectorStd, quint32 p_funcId);
    virtual ~GenVectorFunc();

    quint32 getFuncId() const;
    bool getIsEnabled() const;
    void setIsEnabled(bool p_value);

    void configure(bool p_isEnabled, quint32 p_prodId);

    void pushMsg(ProtMsg* p_pMsg);

    virtual void processReceivedMsg(ProtMsg* p_pMsg) = 0;   ///< this method will be called if this device receives a message
    virtual void configureFunction(quint32 p_prodId) = 0;

    Q_INVOKABLE ComDevice * getComDevice() const;

    QString getFunctionName() const;

    bool getConnectionState();
    QString getConnectionName();

    bool updateConnectedState();    ///< check if a connection is established between this function and one link

    bool getHasReceivedMessage();

signals :
    void isEnabledChanged(bool);
    void comDeviceChanged(ComDevice *);
    void evConnectionStateChanged(bool);
    void evUpdateLinkState();
    void hasReceivedMessageChanged(bool);

protected slots :
    void onDeviceLinkChanged(GenComLink * p_link);
    void onLinkStateChanged();

protected :
    ProtMsg * newMsgEvent(quint32 p_msgCmdEvent);
    ProtMsg * newMsgCmd(quint32 p_msgCmd);
    ProtMsg * newMsgParam(quint32 p_msgType, quint32 p_msgParam);
    virtual void sendMsg(ProtMsg * p_pMsg, bool p_isEcho = false); ///< p_isEcho is true if it's an answer to a SET done by the device function. In this case, only send it to Panel Controls

    virtual void onDeviceConnected(); ///< read device parameters when a device is just connected (called only for Ground Station)

    bool isVectorMpc() const;

    quint32 m_funcId;
    bool m_isEnabled;

    VectorStd * m_pVectorStd;
    ComDevice * m_pComDevice; ///< Device linked
    GenComLink * m_pComLink; ///< Link of the device

    bool m_hasReceivedMessage;
};

// ----------------------------------------------------------------------------
// -- MACRO

//  Q_PROPERTY(T param##NAME READ getParam##NAME WRITE setParam##NAME NOTIFY param##NAME##Changed)

#define FUNC_PARAM(NAME, T, SIZEOF, MSG_PARAM, INIT) \
    FUNC_PARAM_PRE(m_param, NAME, T, SIZEOF, MSG_PARAM, INIT) \
    private : \
    T m_param##NAME = INIT; \
    public : \

// PRE = prefix, NAME_P = name of the Property, NAME_D = name of the Data in the program.
// NAME_P can use index

#define FUNC_PARAM_PRE(PRE, NAME, T, SIZEOF, MSG_PARAM, INIT) \
    FUNC_PARAM_PRE_P_D(PRE, NAME, NAME, T, SIZEOF, MSG_PARAM, INIT)

#define FUNC_PARAM_PRE_P_D(PRE, NAME_P, NAME_D, T, SIZEOF, MSG_PARAM, INIT) \
    Q_PROPERTY(T param##NAME_P READ getParam##NAME_P WRITE setParam##NAME_P NOTIFY param##NAME_P##Changed) \
    public : \
    T getParam##NAME_P() \
    { \
        return PRE##NAME_D; \
    } \
    \
    void setParam##NAME_P(T p_value, bool p_isEcho = false) \
    { \
        if( p_value != PRE##NAME_D ) { \
            PRE##NAME_D = p_value; \
            emit param##NAME_P##Changed(p_value); \
            ProtMsg * pMsg = nullptr; \
            if( !p_isEcho ) { \
                pMsg = newMsgParam(ProtMsg::MSG_TYPE_PARAM_SET, ProtMsg::MSG_PARAM); \
            } else if( (!isVectorMpc()) && p_isEcho ) { \
                pMsg = newMsgParam(ProtMsg::MSG_TYPE_PARAM_VALUE, ProtMsg::MSG_PARAM); \
            } \
            if( nullptr != pMsg ) { \
                pMsg->m_len = SIZEOF; \
                memcpy(pMsg->m_payload, &PRE##NAME_D, SIZEOF); \
                sendMsg(pMsg, p_isEcho); \
            } \
        } \
    } \
    \
    Q_SIGNAL void param##NAME_P##Changed(T p_value); \
    \
    public : \


#define DECODE_MSG_PARAM(NAME, T, SIZEOF, MSG_PARAM) \
    if( static_cast<quint16>(ProtMsg::MSG_PARAM) == p_pMsg->m_msgObj ) { \
        if( ProtMsg::MSG_TYPE_PARAM_SET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType) { \
            T value = *(reinterpret_cast<T*>(p_pMsg->m_payload)); \
            setParam##NAME(value, true); \
        } \
        if( ProtMsg::MSG_TYPE_PARAM_GET == p_pMsg->m_msgType ) { \
            ProtMsg * pMsg = newMsgParam(ProtMsg::MSG_TYPE_PARAM_VALUE, ProtMsg::MSG_PARAM); \
            pMsg->m_len = SIZEOF; \
            memcpy(pMsg->m_payload, &m_param##NAME, SIZEOF); \
            sendMsg(pMsg); \
        } \
    } \

#define DECODE_MSG_PARAM_PRE(PRE, NAME, T, SIZEOF, MSG_PARAM) \
    DECODE_MSG_PARAM_PRE_P_D(PRE, NAME, NAME, T, SIZEOF, MSG_PARAM)

#define DECODE_MSG_PARAM_PRE_P_D(PRE, NAME_P, NAME_D, T, SIZEOF, MSG_PARAM) \
    if( static_cast<quint16>(ProtMsg::MSG_PARAM) == p_pMsg->m_msgObj ) { \
        if( ProtMsg::MSG_TYPE_PARAM_SET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType) { \
            T value = *(reinterpret_cast<T*>(p_pMsg->m_payload)); \
            setParam##NAME_P(value, true); \
        } \
        if( ProtMsg::MSG_TYPE_PARAM_GET == p_pMsg->m_msgType ) { \
            ProtMsg * pMsg = newMsgParam(ProtMsg::MSG_TYPE_PARAM_VALUE, ProtMsg::MSG_PARAM); \
            pMsg->m_len = SIZEOF; \
            memcpy(pMsg->m_payload, &PRE##NAME_D, SIZEOF); \
            sendMsg(pMsg); \
        } \
    } \

