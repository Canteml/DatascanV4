/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : BScanGraph.h
# Author      : Thierry Lechevallier
# Date        : 13/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
#include <jkPlot/MyJkQtPlotter.h>
class Surface1d;

// -- External includes
#include <jkqtplotter/jkqtplotter.h>
#include <jkqtplotter/graphs/jkqtpgeometric.h>
#include <jkqtplotter/graphs/jkqtpimpulses.h>
class JKQTPGeoLine;

// -- Qt includes
#include <QtQuick>
#include <QThread>

class BScanGraph : public QQuickPaintedItem, public XmlSettingsLoggable
{
    Q_OBJECT
public:
    BScanGraph(QQuickItem *parent = 0);
    ~BScanGraph();

    void paint( QPainter* painter) override;

    MyJkQtPlotter * getPlot() const;

    void initAfterItemAdded();

    void scaleXIn();
    void scaleXOut();
    void thicknessYIn();
    void thicknessYOut();

    void setZoomXMode(qint32 p_value);
    qint32 getZoomXMode(void) const;

    enum ZOOM_X_MODE {
        ZOOM_X_AUTO,    ///< scale defined by user, bounds XMIN and XMAX computed, current position visible
        ZOOM_X_USER,    ///< scale defined by user, position set bu user with mouse, current position not visible
        ZOOM_X_FULL     ///< scale auto, bounds auto, current position visible
    };

signals :
    void evPrivateNeedRefresh();
    void evZoomXModeChanged(qint32);

protected:
    void routeMouseEvents( QMouseEvent* event);

    virtual void mousePressEvent( QMouseEvent* event ) override;
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;
    virtual void mouseMoveEvent( QMouseEvent* event ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent* event ) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void hoverMoveEvent(QHoverEvent* event) override;

    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private slots :
    void onPrivateNeedRefresh();
    void onThreadFinished();
    void onUpdateWidgetSize();
    void onTimerRefresh();

    void onPlotMouseMove(double, double);
    void onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton);


    void onUpdateBScan(Surface1d* p_surface);
    void onPositionChanged(qreal);

private :
    void updateAxes(void);
    void needRefresh(bool p_force);

    MyJkQtPlotter* m_pPlotBScan;
    JKQTPDatastore* m_dsBScan;
    JKQTPImpulsesVerticalGraph* m_graphBScan;

    size_t m_columnX;
    size_t m_columnY;
//    qreal * m_pAScanXData;
//    qreal * m_pAScanYData;

    volatile bool m_updateInProgress;

    QTimer * m_timeRefresh;
    qint64 m_lastUpdateEpoc;

    QMouseEvent * m_eMouseMove;

    qreal m_lastPositionM;

    // -- Graph
    JKQTPGeoLine * m_pPositionLine;
    bool m_mouseClicked; ///< if true, graph use click
    qreal m_mouseClickPressX;    ///<  Position where mouse click down
    QPointF m_mouseLastPos;
    qreal m_mouseClickPressAxeXMin;
    qreal m_mouseClickPressAxeXMax;

    // -- Settings
    qreal m_AxeXScaleM;  ///< size on X Axis
    qreal m_AxeYMaxMm;
    qint32 m_zoomXMode;
};
