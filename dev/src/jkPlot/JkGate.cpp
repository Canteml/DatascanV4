/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ---------------------------------------------------------------------------- OK
#
# File name   : JkGate.cpp
# Author      : Thierry Lechevallier
# Date        : 17/08/2021
#
# ==========================================================================*/

// internal includes
#include "JkGate.h"
#include <v2/vector/VectorFuncAScan.h>


bool JkGate::m_isGateClicked = false;

JkGate::JkGate(MyJkQtPlotter* p_pPlot, qint32 p_gateIndex) :
    Loggable("JkGate")
{
    m_pPlot = p_pPlot;
    m_gateIndex = p_gateIndex;
    m_pAScan = nullptr;

    m_isHovered = false;
    m_isClicked = false;
    m_isVisible = true;

    m_elementHovered = 0;
    m_margin = 10;
    m_offsetFromGate1 = 0;
    m_P1X = 0;
    m_P2X = 0;

    if( 0 == m_gateIndex ) {
        // RED GATE
        m_brushNotHovered.setColor(QColor("#AA2222"));
        m_brushHovered.setColor(QColor("#EE5555"));
    } else {
        // GREEN GATE
        m_brushNotHovered.setColor(QColor("#22AA22"));
        m_brushHovered.setColor(QColor("#44CC44"));
    }

    m_pLine = new JKQTPGeoLine(p_pPlot, 30, 30, 40, 40);
    m_pLine->setStyle(m_brushHovered.color(), 1.5);
    p_pPlot->addGraph(m_pLine);

    m_pMarkerBegin = new JKQTPGeoSymbol(p_pPlot, 0, 0, JKQTPFilledCircle, 6, m_brushNotHovered.color(), m_brushHovered.color());
    p_pPlot->addGraph(m_pMarkerBegin);
    m_pMarkerBegin->setVisible(false);

    m_pMarkerEnd = new JKQTPGeoSymbol(p_pPlot, 0, 0, JKQTPFilledCircle, 6, m_brushNotHovered.color(), m_brushHovered.color());
    p_pPlot->addGraph(m_pMarkerEnd);
    m_pMarkerEnd->setVisible(false);

    connect(p_pPlot, SIGNAL(plotMouseMove(double, double)), this, SLOT(onPlotMouseMove(double, double)));
    connect(p_pPlot, SIGNAL(plotMouseClicked(double, double, Qt::KeyboardModifiers , Qt::MouseButton)), this, SLOT(onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)));

    onUpdateGateFromModel();
}

void JkGate::setAScan(VectorFuncAScan *p_pAScan)
{
    if( nullptr != m_pAScan) {
        if( 0 == m_gateIndex ) {
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate0StartChanged, this, &JkGate::onUpdateGateFromModel);
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate0WidthChanged, this, &JkGate::onUpdateGateFromModel);
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate0HeightChanged, this, &JkGate::onUpdateGateFromModel);
        } else {
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate1StartChanged, this, &JkGate::onUpdateGateFromModel);
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate1WidthChanged, this, &JkGate::onUpdateGateFromModel);
            disconnect(m_pAScan,&VectorFuncAScan::paramLGate1HeightChanged, this, &JkGate::onUpdateGateFromModel);
        }
    }

    if( 0 == m_gateIndex ) {
        connect(p_pAScan,&VectorFuncAScan::paramLGate0StartChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
        connect(p_pAScan,&VectorFuncAScan::paramLGate0WidthChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
        connect(p_pAScan,&VectorFuncAScan::paramLGate0HeightChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
    } else {
        connect(p_pAScan,&VectorFuncAScan::paramLGate1StartChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
        connect(p_pAScan,&VectorFuncAScan::paramLGate1WidthChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
        connect(p_pAScan,&VectorFuncAScan::paramLGate1HeightChanged, this, &JkGate::onUpdateGateFromModel, Qt::QueuedConnection);
    }

    m_pAScan = p_pAScan;

    onUpdateGateFromModel();
}

void JkGate::setLineY(qreal y)
{
    m_pLine->setY1(y);
    m_pLine->setY2(y);
    m_pMarkerBegin->setY(y);
    m_pMarkerEnd->setY(y);
    onUpdatePlot();
    updateModel();
}

void JkGate::setGate(qreal p_start, qreal p_width, qreal p_height)
{
    if( !m_isClicked ) {
//        if( m_pGate->getIndex() != 0 ) {
//            // synchro on gate Interface => compute a "shift" from the start
//            p_start = p_start + m_pTestiaDevice->getGate1Interface()->getStart();
//        }

        QPointF p1, p2;
        if( m_pLine->getX1() < m_pLine->getX2() ) {
            p1 = QPointF(p_start, p_height);
            p2 = QPointF(p_start+p_width, p_height);
        } else {
            p2 = QPointF(p_start, p_height);
            p1 = QPointF(p_start+p_width, p_height);
        }

        m_P1X = p1.x();
        m_P2X = p2.x();


        m_pLine->setX1(m_P1X + m_offsetFromGate1);
        m_pLine->setY1(p1.y());
        m_pLine->setX2(m_P2X + m_offsetFromGate1);
        m_pLine->setY2(p2.y());

        m_pMarkerBegin->setX(m_P1X + m_offsetFromGate1);
        m_pMarkerBegin->setY(p1.y());

        m_pMarkerEnd->setX(m_P2X + m_offsetFromGate1);
        m_pMarkerEnd->setY(p2.y());

//        JKQTPGeoLine * l = new JKQTPGeoLine(m_pPlot, 0, 0, 10, 10);

//        l->setX1(m_P1X + m_offsetFromGate1);
//        l->setY1(p1.y());
//        l->setX2(m_P2X + m_offsetFromGate1);
//        l->setY2(p2.y());

//        l->setStyle(m_brushHovered.color(), 1.5);
//        m_pPlot->addGraph(l);

        onUpdatePlot();
        emit evGateChanged();
    }
}

void JkGate::setP1(const QPointF &p_value)
{
    m_P1X = p_value.x();

    m_pLine->setX1(m_P1X + m_offsetFromGate1);
    m_pLine->setY1(p_value.y());

    m_pMarkerBegin->setX(m_P1X + m_offsetFromGate1);
    m_pMarkerBegin->setY(p_value.y());

    updateModel();
    onUpdatePlot();

    emit evGateChanged();
}

void JkGate::setP2(const QPointF &p_value)
{
    m_P2X = p_value.x();

    m_pLine->setX2(m_P2X + m_offsetFromGate1);
    m_pLine->setY2(p_value.y());

    m_pMarkerEnd->setX(m_P2X + m_offsetFromGate1);
    m_pMarkerEnd->setY(p_value.y());

    updateModel();
    onUpdatePlot();

    emit evGateChanged();
}

QLineF JkGate::getLine(void)
{
    return QLineF(m_P1X/*m_pLine->getX1()*/, m_pLine->getY1(), m_P2X/*m_pLine->getX2()*/, m_pLine->getY2());
}

qreal JkGate::getGateStart() const
{
    qreal v = qMin(m_P1X, m_P2X); //m_pLine->getX1(), m_pLine->getX2());
//    if( m_pGate->getIndex() != 0 ) {
//        // synchro on gate 0 => compute a "shift" from the start
//        v = v - m_pTestiaDevice->getGate1Interface()->getStart();
//    }
    //qDebug()<<"getGateStart = "<<qMin(m_pLine->getX1(), m_pLine->getX2());
    // round 2 decimals
    v = static_cast<double>(static_cast<int>(v*100+0.5))/100.0;
    return v;
}

qreal JkGate::getGateWidth() const
{
    //qDebug()<<"getGateWidth = "<<qAbs(m_pLine->getX1() - m_pLine->getX2());
    qreal v = qAbs(m_pLine->getX1() - m_pLine->getX2());
    // round 2 decimals
    v = static_cast<double>(static_cast<int>(v*100+0.5))/100.0;
    return v;
}

qreal JkGate::getGateHeight() const
{
    qreal v = m_pLine->getY1();
    // round 2 decimals
    v = static_cast<double>(static_cast<int>(v*100+0.5))/100.0;
    return v;
}

qreal JkGate::getCurrentOffsetGate1() const
{
    return m_offsetFromGate1;
}

void JkGate::setCurrentOffsetGate1(qreal p_value)
{
    m_offsetFromGate1 = p_value;
}

void JkGate::refreshXPositionOffset()
{
    m_pLine->setX1(m_P1X + m_offsetFromGate1);
    m_pLine->setX2(m_P2X + m_offsetFromGate1);
    m_pMarkerBegin->setX(m_P1X + m_offsetFromGate1);
    m_pMarkerEnd->setX(m_P2X + m_offsetFromGate1);
}


void JkGate::setMargin(qreal p_value)
{
    m_margin = p_value;
}

qreal JkGate::getMargin() const
{
    return m_margin;
}

void JkGate::mouseMoveEvent(const QPointF &p_position)
{
//    if( ProcessManager::MODE_CALIB == m_pProcessManager->getMode() ||
//        ProcessManager::MODE_SETTING == m_pProcessManager->getMode() ) {
        qreal dL = distLine(p_position);
        qreal dP1 = distMarkerBegin(p_position);
        qreal dP2 = distMarkerEnd(p_position);


        if( m_isClicked ) {
            if( 1 == m_elementHovered ) {
                setLineY(p_position.y());//m_pLine->getX1(), p_position.y(), m_pLine->getX2(), p_position.y());
            }
            if( 2 == m_elementHovered ) {
                setP1(QPointF(p_position.x()-m_offsetFromGate1, m_pLine->getY1()));
            }
            if( 3 == m_elementHovered ) {
                setP2(QPointF(p_position.x()-m_offsetFromGate1, m_pLine->getY2()));
            }
        } else {
            if( dP1 < dP2  && dP1 < m_margin ) {
                m_elementHovered = 2;
                m_isHovered = true;
                onUpdatePlot();
            } else if( dP2 < m_margin ) {
                m_elementHovered = 3;
                m_isHovered = true;
                onUpdatePlot();
            } else if( dL < m_margin ){
                m_elementHovered = 1;
                m_isHovered = true;
                onUpdatePlot();
            } else {
                if( m_isHovered ) {
                    m_elementHovered = 0;
                    m_isHovered = false;
                    onUpdatePlot();
                }
            }
        }
    //}
    //qDebug()<<"MOVE dL = "<<dL<<" / dP1 = "<<dP1<<" / dP2 = "<<dP2<<" / m_elementHovered = "<<m_elementHovered;
}

void JkGate::mouseClickEvent(const QPointF &p_position)
{
    if( !m_isGateClicked && !m_pPlot->getMouseUsedByItem() /*&&
            ( ProcessManager::MODE_CALIB == m_pProcessManager->getMode() ||
              ProcessManager::MODE_SETTING == m_pProcessManager->getMode()) */) {
        qreal dL = distLine(p_position);
        qreal dP1 = distMarkerBegin(p_position);
        qreal dP2 = distMarkerEnd(p_position);

        //qDebug()<<"dL = "<<dL<<" / dP1 = "<<dP1<<" / dP2 = "<<dP2;

        if( dP1 < dP2  && dP1 < m_margin ) {
            m_elementHovered = 2;
            m_isClicked = true;
            m_pPlot->setMouseUsedByItem(true);
            m_isGateClicked = true;
            m_isHovered = true;
            onUpdatePlot();
        } else if( dP2 < m_margin ) {
            m_elementHovered = 3;
            m_isClicked = true;
            m_pPlot->setMouseUsedByItem(true);
            m_isGateClicked = true;
            m_isHovered = true;
            onUpdatePlot();
        } else if( dL < m_margin ){
            m_elementHovered = 1;
            m_isClicked = true;
            m_pPlot->setMouseUsedByItem(true);
            m_isGateClicked = true;
            m_isHovered = true;
            onUpdatePlot();
        }
    }

}

void JkGate::mouseReleaseEvent(const QPointF &p_position)
{
    if( m_isClicked ) {
        m_isClicked = false;
        m_pPlot->setMouseUsedByItem(false);
        m_isGateClicked = false;
        onUpdatePlot();
    }

    mouseMoveEvent(p_position);
}

bool JkGate::getIsHovered()
{
    return m_isHovered;
}

bool JkGate::getIsClicked()
{
    return m_isClicked;
}

bool JkGate::getIsVisible() const
{
    return m_isVisible;
}

void JkGate::setIsVisible(bool p_value)
{
    if( m_isVisible != p_value ) {
        m_isVisible = p_value;
        onUpdatePlot();
    }
}

qreal JkGate::distLine(const QPointF &p) const
{
    qreal d = m_margin * 10;
    // use pixels :
    QPointF pp (m_pPlot->getXAxis()->x2p(p.x()), m_pPlot->getYAxis()->x2p(p.y()));

    // transform to local coordinates system (0,0) - (lx, ly)

    QPointF p1(m_pPlot->getXAxis()->x2p(m_pLine->getX1()), m_pPlot->getYAxis()->x2p(m_pLine->getY1()));
    QPointF p2(m_pPlot->getXAxis()->x2p(m_pLine->getX2()), m_pPlot->getYAxis()->x2p(m_pLine->getY2()));

    // Rectangle
    QRectF r(qMin(p1.x(), p2.x()) - m_margin, qMin(p1.y(), p2.y()) - m_margin,
             qAbs(p1.x() - p2.x()) + 2 * m_margin, qAbs(p1.y() - p2.y()) + 2 * m_margin);

    if( r.contains(pp) ) {
        qreal x = pp.x() - p1.x();
        qreal y = pp.y() - p1.y();
        qreal x2 = p2.x() - p1.x();
        qreal y2 = p2.y() - p1.y();

        // if line is a point (nodes are the same) =>
        // just return distance between point and one line node
        qreal norm = sqrt(x2*x2 + y2*y2);
        if (norm <= std::numeric_limits<int>::epsilon()) {
            d = sqrt(x*x + y*y);
        } else {
            d = fabs(x*y2 - y*x2) / norm;
        }
    }

    // distance
    return d;
}

qreal JkGate::distMarkerBegin(const QPointF &p) const
{
    QPointF pp (m_pPlot->getXAxis()->x2p(p.x()), m_pPlot->getYAxis()->x2p(p.y()));
    QPointF p1(m_pPlot->getXAxis()->x2p(m_pLine->getX1()), m_pPlot->getYAxis()->x2p(m_pLine->getY1()));

    return sqrt( pow((p1.x() - pp.x()), 2) + pow((p1.y() - pp.y()), 2));
}

qreal JkGate::distMarkerEnd(const QPointF &p) const
{
    QPointF pp (m_pPlot->getXAxis()->x2p(p.x()), m_pPlot->getYAxis()->x2p(p.y()));
    QPointF p2(m_pPlot->getXAxis()->x2p(m_pLine->getX2()), m_pPlot->getYAxis()->x2p(m_pLine->getY2()));

    return sqrt( pow((p2.x() - pp.x()), 2) + pow((p2.y() - pp.y()), 2));
}

void JkGate::onUpdateGateFromModel()
{
    if( nullptr != m_pAScan ) {
        if( 0 == m_gateIndex ) {
            setGate(m_pAScan->getParamLGate0Start(), m_pAScan->getParamLGate0Width(), m_pAScan->getParamLGate0Height());
        } else {
            setGate(m_pAScan->getParamLGate1Start(), m_pAScan->getParamLGate1Width(), m_pAScan->getParamLGate1Height());
        }
    }
}

void JkGate::onPlotMouseMove(double x, double y)
{
    mouseMoveEvent(QPointF(x, y));
}

void JkGate::onPlotMouseClicked(double x, double y, Qt::KeyboardModifiers /*modifiers*/, Qt::MouseButton /*button*/)
{
    if( !getIsClicked() && !m_isGateClicked ) {
        mouseClickEvent(QPointF(x, y));
    } else {
        mouseReleaseEvent(QPointF(x, y));
    }
}

void JkGate::onUpdatePlot()
{
    if( !m_isVisible ) {
        m_pMarkerBegin->setVisible(false);
        m_pMarkerEnd->setVisible(false);
        m_pLine->setVisible(false);
    } else {
        m_pLine->setVisible(true);
        m_pMarkerBegin->setVisible( 2 == m_elementHovered );
        m_pMarkerEnd->setVisible( 3 == m_elementHovered );

        if( 0 == m_elementHovered ) {
            m_pLine->setColor(m_brushNotHovered.color());
            m_pMarkerBegin->setSymbolColor(m_brushNotHovered.color());
            m_pMarkerEnd->setSymbolColor(m_brushNotHovered.color());
        } else {
            m_pLine->setColor(m_brushHovered.color());
            m_pMarkerBegin->setSymbolColor(m_brushHovered.color());
            m_pMarkerEnd->setSymbolColor(m_brushHovered.color());
        }

        if( m_isClicked ) {
            m_pLine->setLineStyle(Qt::DashLine);
        } else {
            m_pLine->setLineStyle(Qt::SolidLine);
        }
    }
    m_pPlot->requestUpdate();
}

void JkGate::updateModel()
{
    if( 0 == m_gateIndex ) {
        if( m_pAScan->getParamLGate0Start() != getGateStart() ) {
            m_pAScan->setParamLGate0Start((qint32)getGateStart());
        }
        if( m_pAScan->getParamLGate0Width() != getGateWidth() ) {
            m_pAScan->setParamLGate0Width((qint32)getGateWidth());
        }
        if( m_pAScan->getParamLGate0Height() != getGateHeight() ) {
            m_pAScan->setParamLGate0Height((qint32)getGateHeight());
        }
    } else {
        if( m_pAScan->getParamLGate1Start() != getGateStart() ) {
            m_pAScan->setParamLGate1Start((qint32)getGateStart());
        }
        if( m_pAScan->getParamLGate1Width() != getGateWidth() ) {
            m_pAScan->setParamLGate1Width((qint32)getGateWidth());
        }
        if( m_pAScan->getParamLGate1Height() != getGateHeight() ) {
            m_pAScan->setParamLGate1Height((qint32)getGateHeight());
        }
    }
}
