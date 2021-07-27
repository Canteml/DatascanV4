/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : MyJkQtPlotter.h
# Author      : Thierry Lechevallier
# Date        : 05/01/2020
#
# ==========================================================================*/

#pragma once


// -- Internal includes
#include <log/Loggable.h>

// -- External Includes

#include <jkqtplotter/jkqtplotter.h>


class MyJkQtPlotter : public JKQTPlotter, public Loggable
{
    Q_OBJECT
public:
    MyJkQtPlotter();
    ~MyJkQtPlotter();

    void requestUpdate();

    // -- properties
    bool getMouseEnabled() const;
    void setMouseEnabled(bool p_value);


protected :
    // -- inherited from QWidget
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;

private :
    qint32 minTimeBetweenUpdate() const;

signals :
    void evRequestUpdate();


private slots :
    void onRequestUpdate();

private :
    // -- Properties
    bool m_mouseEnabled;    ///< if false : any mouse action is ignored (mouse enabled by default)
    volatile bool m_flagUpdatePending;   ///< a request must be done after the current running update
    qint64 m_timeLastUpdateDone;    ///< epoc time (ms)
    QTimer * m_timerUpdateDelayed;
};
