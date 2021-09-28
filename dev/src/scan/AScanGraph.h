/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AScanGraph.h
# Author      : Thierry Lechevallier
# Date        : 28/07/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
#include <jkPlot/MyJkQtPlotter.h>
#include <v2/scan/AScan.h>
// -- External includes
#include <jkqtplotter/jkqtplotter.h>
#include <jkqtplotter/graphs/jkqtpgeometric.h>
class JKQTPGeoLine;

// -- Qt includes
#include <QtQuick>
#include <QThread>

class AScanGraphWorker;

class AScanGraph : public QQuickPaintedItem, public XmlSettingsLoggable
{
    friend AScanGraphWorker;
    Q_OBJECT
public:
    AScanGraph(QQuickItem *parent = 0);
    ~AScanGraph();

    void paint( QPainter* painter) override;
    void stop();

    void updateAScan(AScan *);

    MyJkQtPlotter * getPlot() const;

    void initAfterItemAdded();

    void zoomIn();
    void zoomOut();

signals :
    void evPrivateNeedRefresh();

protected:
    void computeTickSpacing();
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

    void onAScanReplotted();

    void onPlotMouseMove(double, double);
    void onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton);

private :
    void needRefresh(bool p_force);
    void draw();

    MyJkQtPlotter *m_pPlotAScan;
    JKQTPDatastore* m_dsAScan;
    JKQTPXYLineGraph* m_graphAScan;

    size_t m_columnX;
    size_t m_columnY;
    qreal m_pAScanXData[AScanRawSample::ASCAN_SAMPLE_SIZE];
    qreal m_pAScanYData[AScanRawSample::ASCAN_SAMPLE_SIZE];

    volatile bool m_updateInProgress;

    QTimer * m_timeRefresh;
    qint64 m_lastUpdateEpoc;
    QMouseEvent * m_eMouseMove;

    // -- Graph
    JKQTPGeoLine * m_pZeroLine;
    bool m_mouseClicked; ///< if true, graph use click
    qreal m_mouseClickPressX;    ///<  Position where mouse click down
    QPointF m_mouseLastPos;
    qreal m_mouseClickPressAxeXMin;
    qreal m_mouseClickPressAxeXMax;


    // -- Settings
    qreal m_AxeXMin;
    qreal m_AxeXMax;

    // -- Render
    AScanGraphWorker * m_pAScanGraphWorker;
    QImage m_buffer[2]; ///< double buffering
    qint32 m_curBuffer; ///< index of the buffer to display (render is done in index (1-m_curBuffer))
    QMutex m_mutBuffer; ///< protect buffer swapping
};

// ---------------------------

class AScanGraphWorker : public QObject, public Loggable
{
    Q_OBJECT
public :
    AScanGraphWorker(AScanGraph * p_parent);
    ~AScanGraphWorker();

    void requestUpdate();
    void signalPaintDone();
    void start();
    void stop();
    bool isBufferReady() const;
    QImage * getOldBuffer();

signals :

    void evRequestUpdate();
    void evUpdateDone();

private slots :
    void onProcessUpdate();

private :
    AScanGraph * m_pParent;
    volatile bool m_requestUpdate;
    volatile bool m_processing;
    QThread * m_pThread;
};

