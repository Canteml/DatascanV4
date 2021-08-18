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
#include <log/Loggable.h>
#include <jkPlot/MyJkQtPlotter.h>
class AScan;
class AScanGraphWorker;

// -- External includes
#include <jkqtplotter/jkqtplotter.h>
#include <jkqtplotter/graphs/jkqtpgeometric.h>

// -- Qt includes
#include <QtQuick>
#include <QThread>

class AScanGraph : public QQuickPaintedItem, public Loggable
{
    Q_OBJECT
public:
    AScanGraph(QQuickItem *parent = 0);
    ~AScanGraph();

    void paint( QPainter* painter);

    void updateAScan(AScan *);

    MyJkQtPlotter * getPlot() const;

signals :
    //void evPrivateNeedRefresh();

protected:
    void routeMouseEvents( QMouseEvent* event);

    virtual void mousePressEvent( QMouseEvent* event ) override;
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;
    virtual void mouseMoveEvent( QMouseEvent* event ) override;
    virtual void mouseDoubleClickEvent( QMouseEvent* event ) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void hoverMoveEvent(QHoverEvent* event) override;

private slots :
    void onThreadFinished();
    void onUpdateWidgetSize();
    void onTimerRefresh();
    void onRedrawDone();

private :
    void onNeedRefresh(bool p_force);

    MyJkQtPlotter *m_pPlotAScan;
    JKQTPDatastore* m_dsAScan;
    JKQTPXYLineGraph* m_graphAScan;

    size_t m_columnX;
    size_t m_columnY;
    qreal * m_pAScanXData;
    qreal * m_pAScanYData;

    bool m_updateInProgress;

    QThread * m_pWorker;
    QTimer * m_timeRefresh;
    qint64 m_lastUpdateEpoc;
    AScanGraphWorker * m_pAScanGraphWorker;

    QMouseEvent * m_eMouseMove;
};



class AScanGraphWorker : public QObject, public Loggable
{
    Q_OBJECT
public :
    AScanGraphWorker(MyJkQtPlotter *p_pPlotAScan);
    ~AScanGraphWorker();
    void requestRedraw();
    void notifyUpdate();
    bool waitingUpdate() const;
    bool isProcessing() const;
    bool isPendingUpdate() const;
    bool isUsingPaintDevice() const;

signals:
    void evRequestRedraw();
    void evRedrawDone();

private slots :
    void onRequestRedraw();

private :
    MyJkQtPlotter *m_pPlotAScan;
    QThread * m_pThread;
    volatile bool m_processingRedraw;
    volatile bool m_pendingRedraw;
    volatile bool m_waitingUpdate;
    volatile bool m_usingPaintDevice;
};
