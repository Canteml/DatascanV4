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
    m_flagUpdatePending = false;
    m_timeLastUpdateDone = QDateTime::currentMSecsSinceEpoch();

    m_timerUpdateDelayed = new QTimer();
    m_timerUpdateDelayed->setSingleShot(true);

    connect(m_timerUpdateDelayed, &QTimer::timeout, this, &MyJkQtPlotter::onRequestUpdate);
    connect(this, &MyJkQtPlotter::evRequestUpdate, this, &MyJkQtPlotter::onRequestUpdate, Qt::QueuedConnection);
}

MyJkQtPlotter::~MyJkQtPlotter()
{
    if( m_timerUpdateDelayed->isActive() ) {
        m_timerUpdateDelayed->stop();
    }
    delete m_timerUpdateDelayed;
    m_timerUpdateDelayed = nullptr;
}

void MyJkQtPlotter::requestUpdate()
{
//    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
//    if( curTime-m_timeLastUpdateDone > minTimeBetweenUpdate() && !m_timerUpdateDelayed->isActive() ) {
//        onRequestUpdate();
//    } else {
//        if( !m_flagUpdatePending && !m_timerUpdateDelayed->isActive() ) {
//        m_flagUpdatePending = true;
//            if( !m_timerUpdateDelayed->isActive() ) {
//                // start a timer to request an update later
//                m_timerUpdateDelayed->start(minTimeBetweenUpdate()-(curTime-m_timeLastUpdateDone));
//            }
//        }
//    }
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
        fLogDebug1("move in");
        JKQTPlotter::mouseMoveEvent(event);
        fLogDebug1("move out");
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
//    if( m_mouseEnabled ) {
//        JKQTPlotter::moveEvent(event);
//        fLogDebug1("move !");
//        if( !event->isAccepted() ) {
//        }
//    }
}

qint32 MyJkQtPlotter::minTimeBetweenUpdate() const
{
    return 1000/60;
}

void MyJkQtPlotter::onRequestUpdate()
{
    m_flagUpdatePending = false;
    redrawPlot();
    // update last time of update
    m_timeLastUpdateDone = QDateTime::currentMSecsSinceEpoch();
}
