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

class BScanWorker;

class BScanGraph : public QQuickPaintedItem, public XmlSettingsLoggable
{
    friend BScanWorker;
    Q_OBJECT
public:
    BScanGraph(QQuickItem *parent = 0);
    ~BScanGraph();

    void paint( QPainter* painter) override;

    MyJkQtPlotter * getPlot() const;

    void initAfterItemAdded();
    void processLastSurface();
    void processZoom();

//    void scaleXIn();
//    void scaleXOut();
//    void thicknessYIn();
//    void thicknessYOut();

    // -- Properties

    qint32 getViewXMode(void) const;
    void setViewXMode(qint32 p_value);

    qint32 getViewYMode(void) const;
    void setViewYMode(qint32 p_value);

    qreal getAxeXScaleMm(void) const;
    void setAxeXScaleMm(qreal p_value);

    qreal getAxeYMinMm(void) const;
    void setAxeYMinMm(qreal p_value);

    qreal getAxeYMaxMm(void) const;
    void setAxeYMaxMm(qreal p_value);

    // -- Actions

    void axeXScaleIn();
    void axeXScaleOut();

    void updateAxes();

    enum VIEW_X_MODE {
        VIEW_X_AUTO,    ///< scale defined by user, bounds XMIN and XMAX computed, current position visible (default)
        VIEW_X_USER,    ///< scale defined by user, position set bu user with mouse, current position not visible
        VIEW_X_FULL     ///< scale auto, bounds auto, current position visible
    };

    enum VIEW_Y_MODE {
        VIEW_Y_AUTO,    ///< Y scale shows the range from measure min to measures max (not 0)
        VIEW_Y_USER,    ///< scale defined by user, from 0 to max value defined by user
        VIEW_Y_FULL     ///< scale from 0 to measure max
    };

    static const qreal SCALE_X_M_MIN;
    static const qreal SCALE_X_M_MAX;
    static const qreal SCALE_Y_MM_MAX;

signals :
    void evPrivateNeedRefresh();
    void evViewXModeChanged(qint32);
    void evViewYModeChanged(qint32);
    void evAxeXScaleMmChanged(qreal);
    void evAxeYMinMmChanged(qreal);
    void evAxeYMaxMmChanged(qreal);

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

    void onBScanReplotted();

private :

    void needRefresh(bool p_force);
    void draw();
    void resetFlags();

    volatile bool m_updateRequested;
    volatile bool m_updateInProgress;

    QTimer * m_timeRefresh;
    qint64 m_lastRequestEpoc;

    QMouseEvent * m_eMouseMove;

    // -- Graph
    JKQTPGeoLine * m_pPositionLine;
    MyJkQtPlotter* m_pPlotBScan;
    JKQTPDatastore* m_dsBScan;
    JKQTPImpulsesVerticalGraph* m_graphBScan;

    size_t m_columnX;
    size_t m_columnY;

    // -- Mouse
    bool m_mouseClicked; ///< if true, graph use click
    qreal m_mouseClickPressX;    ///<  Position where mouse click down
    QPointF m_mouseLastPos;
    qreal m_mouseClickPressAxeXMin;
    qreal m_mouseClickPressAxeXMax;

    // -- Settings
    // Data saved
    qreal m_axeXScaleM;
    qreal m_axeYMinMm;
    qreal m_axeYMaxMm;

    // -- Data
    qreal m_lastPositionM;
    BScanWorker * m_pBScanWorker;
    Surface1d* m_pLastSurface1d;

    qint32 m_viewXMode; ///< value in VIEW_X_MODE
    qint32 m_viewYMode; ///< value in VIEW_Y_MODE

    qreal m_axeXUserMinM;   ///< used if VIEW_X_MODE = VIEW_X_USER, completed by m_axeXScaleM
    qreal m_axeYUserMinM;   ///< used if VIEW_Y_MODE = VIEW_Y_USER
    qreal m_axeYUserMaxM;

    // Range of data
    qreal m_surfaceXMinMm;
    qreal m_surfaceXMaxMm;
    qreal m_surfaceYMinMm;
    qreal m_surfaceYMaxMm;

//    // Visible area of BScan
//    qreal m_viewXMinM;
//    qreal m_viewXMaxM;
//    qreal m_viewYMinM;
//    qreal m_viewYMaxM;

    // draw flags
    volatile bool m_drawFlagPosition;           ///< position changed since last draw
    volatile bool m_drawFlagViewXMode;        ///< view X mode changed since last draw
    volatile bool m_drawFlagViewYMode;        ///< view X mode changed since last draw
    volatile bool m_drawFlagSurfaceRange;       ///< one of the surface range dimension have changed (m_surfaceX/Y/Min/Max)
    volatile bool m_drawFlagUserAxeX;          ///< Mouse has moved Axe X
    volatile bool m_drawFlagUserAxeY;          ///< Mouse has moved Axe X

};


class BScanWorker : public QObject, public Loggable
{
    Q_OBJECT
public :
    BScanWorker(BScanGraph * p_parent);
    ~BScanWorker();

    void requestUpdate();
    void signalPaintDone();

signals :

    void evRequestUpdate();
    void evUpdateDone();

private slots :
    void onProcessUpdate();

private :
    BScanGraph * m_pParent;
    volatile bool m_requestUpdate;
    volatile bool m_processing;
    QThread * m_pThread;
};
