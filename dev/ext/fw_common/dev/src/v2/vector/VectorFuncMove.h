/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncMove.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// -- Qt includes
#include <QTimer>

// ----------------------------------------------------------------------------
// -- VectorFuncMove

/**
 * @brief The VectorFuncAScan class
 */
class VectorFuncMove : public GenVectorFunc {
    Q_OBJECT

    Q_PROPERTY(quint8 moveDir READ getMoveDir NOTIFY moveDirChanged)

    Q_PROPERTY(bool IsManualMode READ getManualMode NOTIFY manualModeChanged)

    QML_ELEMENT

public :

    VectorFuncMove(VectorStd *p_pVectorStd);
    ~VectorFuncMove();

    enum MOVE_CMD  {
        MOVE_NO_CMD,
        MOVE_CMD_JOY_X_Y,
        MOVE_CMD_JOY_X_Y_AUTO
    };

    enum MOVE_DIRECTION  {
        MOVE_NO_DIRECTION = 0,
        MOVE_DIR_LEFT =     0b00000001,
        MOVE_DIR_RIGHT =    0b00000010,
        MOVE_DIR_UP =       0b00000100,
        MOVE_DIR_DOWN =     0b00001000
    };

    static const qreal MOVE_KEY_FACTOR; ///< limit key moves
    static const quint32 NO_MOVEMENT_UPDATE_MS_INTERVAL;

    // -- PARAM
    void setJoyXY(qint8 p_x, qint8 p_y);

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    // -- CONTROL
     Q_INVOKABLE void setManualMode(bool ismanual);

    Q_INVOKABLE void moveKeyLeft();
    Q_INVOKABLE void moveKeyRight();
    Q_INVOKABLE void moveKeyUp();
    Q_INVOKABLE void moveKeyDown();
    Q_INVOKABLE void moveBoost(bool p_value);

    Q_INVOKABLE void moveKeyUpState(bool state);
    Q_INVOKABLE void moveKeyDownState(bool state);
    Q_INVOKABLE void moveKeyLeftState(bool state);
    Q_INVOKABLE void moveKeyRightState(bool state);

    Q_INVOKABLE void moveKeyUpdate();

    Q_INVOKABLE void stopMove();
    Q_INVOKABLE void moveStopX();
    Q_INVOKABLE void moveStopY();

    quint8 getMoveDir() const;

    bool getManualMode() const;

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
    void processPendingCmd();

private slots :
    void onCmdAnsTimeout();
    void onNoMovementUpdate();

signals :
    void evCmdJoyXY(qint8 p_x, qint8 p_y);
    void moveDirChanged(quint8);   ///< value in MOVE_DIRECTION
    void manualModeChanged(bool);

private :
    QTimer* m_tCmdAnsTimeout;
    QTimer* m_tNoMovementUpdate;
    volatile qint8 m_pendingCmd; ///< value in MOVE_CMD, store la
    qint8 m_pendingJoyX;
    qint8 m_pendingJoyY;
    bool m_joyInit;
    quint8 m_moveDir; ///< value in MOVE_DIRECTION (global value)
    quint8 m_moveKeyDir; ///< value in MOVE_DIRECTION (key control)
    bool m_moveBoost;
    bool m_manualMode;
};
