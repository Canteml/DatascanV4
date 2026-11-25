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
    m_mouseUsedByItem = false;
}

MyJkQtPlotter::~MyJkQtPlotter()
{

}

void MyJkQtPlotter::requestUpdate()
{
    emit evRequestUpdate();
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
    // double click intentionally ignored (strange behaviour in JKQTPlotter)
}

void MyJkQtPlotter::mouseMoveEvent(QMouseEvent *event)
{
    JKQTPlotter::mouseMoveEvent(event);
}

void MyJkQtPlotter::mousePressEvent(QMouseEvent *event)
{
    JKQTPlotter::mousePressEvent(event);
}

void MyJkQtPlotter::mouseReleaseEvent(QMouseEvent *event)
{
    JKQTPlotter::mouseReleaseEvent(event);
}

void MyJkQtPlotter::wheelEvent(QWheelEvent *)
{

}

void MyJkQtPlotter::moveEvent(QMoveEvent */*event*/)
{
    // Never called with QML => hovered
}

void MyJkQtPlotter::resizeEvent(QResizeEvent *event)
{
    JKQTPlotter::resizeEvent(event);
    emit evRequestUpdate(); // force update after change of size
}

