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

    bool getMouseUsedByItem() const;
    void setMouseUsedByItem(bool p_used);///< should be called by items (like "Gates") to say that mouse event are catched


protected :
    // -- inherited from QWidget
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent * event) override;
    virtual void moveEvent(QMoveEvent *event) override;

private :
    qint32 minTimeBetweenUpdate() const;

signals :
    void evRequestUpdate();

private :
    // -- Properties
    bool m_mouseEnabled;    ///< if false : any mouse action is ignored (mouse enabled by default)
    bool m_mouseUsedByItem;       ///< if mouse is not used by any item, Graph can manage event (move in graph ..)
};
