/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : JkGate.h
# Author      : Thierry Lechevallier
# Date        : 07/12/2020
#
# ==========================================================================*/

#pragma once

// -- internal includes
#include <log/Loggable.h>
#include <jkPlot/MyJkQtPlotter.h>
class VectorFuncAScan;

// -- Qt includes
#include <QWidget>

// -- Jk includes
#include <jkqtplotter/jkqtplotter.h>
#include <jkqtplotter/graphs/jkqtpimage.h>
#include <jkqtplotter/graphs/jkqtpgeometric.h>


class JkGate : public QObject, public Loggable {
    Q_OBJECT

public :
    JkGate(MyJkQtPlotter* p_pPlot, qint32 p_gateIndex);

    void setAScan(VectorFuncAScan * p_pAScan);

    void setLine(const QLineF & p_value);
    void setLineY(qreal y);//setLine(qreal x1, qreal y1, qreal x2, qreal y2);
    void setGate(qreal p_start, qreal p_width, qreal p_height);
    void setP1(const QPointF & p_value);
    void setP2(const QPointF & p_value);
    QLineF getLine(void);
    qreal getGateStart() const;
    qreal getGateWidth() const;
    qreal getGateHeight() const;

    qreal getCurrentOffsetGate1() const;
    void setCurrentOffsetGate1(qreal p_value);
    void refreshXPositionOffset();

    void setMargin(qreal p_value);
    qreal getMargin() const;

    void mouseMoveEvent(const QPointF & p_position);
    void mouseClickEvent(const QPointF & p_position);
    void mouseReleaseEvent(const QPointF &p_position);

    bool getIsHovered();
    bool getIsClicked();

    bool getIsVisible() const;
    void setIsVisible(bool p_value);

    qreal distLine(const QPointF &p) const;
    qreal distMarkerBegin(const QPointF &p) const;
    qreal distMarkerEnd(const QPointF &p) const;

signals:
    void evIsHoveredChanged();
    void evIsClicked();
    void evGateChanged();

private slots :
    void onUpdateGateFromModel();
    void onPlotMouseMove(double, double);
    void onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton);
    void onUpdatePlot();

private :

    void updateModel();

    JKQTPGeoLine * m_pLine;
    JKQTPGeoSymbol * m_pMarkerBegin;
    JKQTPGeoSymbol * m_pMarkerEnd;

    QBrush m_brushNotHovered;
    QBrush m_brushHovered;


    MyJkQtPlotter *m_pPlot;
    VectorFuncAScan* m_pAScan;

    bool m_isHovered;
    bool m_isClicked;
    bool m_isVisible;
    qint32 m_gateIndex; ///< 0 or 1

    static bool m_isGateClicked;    ///< only one gate can be clicked at a time

    qreal m_margin;
    qreal m_offsetFromGate1;

    qreal m_P1X; ///< value computed from testia gate, without consideration of SynchroGate1
    qreal m_P2X; ///< value computed from testia gate, without consideration of SynchroGate1


    quint32 m_elementHovered; // 0 = nothing, 1 = line, 2 = begin marker, 3 = end marker
};
