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

#include "MyJkQtPlotter.h"

#include <QDateTime>

MyJkQtPlotter::MyJkQtPlotter() : Loggable("MyJkQtPlotter")
{
    m_mouseEnabled = true;
    m_mouseUsedByItem = false;
}

MyJkQtPlotter::~MyJkQtPlotter()
{

}

void MyJkQtPlotter::requestUpdate()
{
    emit evRequestUpdate();
}

// -- properties
bool MyJkQtPlotter::getMouseEnabled() const
{
    return m_mouseEnabled;
}

void MyJkQtPlotter::setMouseEnabled(bool p_value)
{
    m_mouseEnabled = p_value;
}

bool MyJkQtPlotter::getMouseUsedByItem() const
{
    return m_mouseUsedByItem;
}

void MyJkQtPlotter::setMouseUsedByItem(bool p_used)
{
    m_mouseUsedByItem = p_used;
}


// -- inherited from QWidget
void MyJkQtPlotter::mouseDoubleClickEvent(QMouseEvent */*event*/)
{
    if( m_mouseEnabled ) {
        // strange behaviour with double click
        //JKQTPlotter::mouseDoubleClickEvent(event);
    }
}

void MyJkQtPlotter::mouseMoveEvent(QMouseEvent *event)
{
    if( m_mouseEnabled ) {
        JKQTPlotter::mouseMoveEvent(event);
    }
}

void MyJkQtPlotter::mousePressEvent(QMouseEvent *event)
{
    if( m_mouseEnabled ) {
        JKQTPlotter::mousePressEvent(event);
    }
}

void MyJkQtPlotter::mouseReleaseEvent(QMouseEvent *event)
{
    if( m_mouseEnabled ) {
        JKQTPlotter::mouseReleaseEvent(event);
    }
}

void MyJkQtPlotter::wheelEvent(QWheelEvent *)
{

}

void MyJkQtPlotter::moveEvent(QMoveEvent */*event*/)
{
    // Never called with QML => hovered
}

qint32 MyJkQtPlotter::minTimeBetweenUpdate() const
{
    return 1000/60;
}

