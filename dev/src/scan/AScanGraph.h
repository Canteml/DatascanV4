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

signals :
    void evPrivateNeedRefresh();

private slots :
    void onNeedRefresh();
    void onThreadFinished();
    void onUpdateWidgetSize();
    void onTimerRefresh();
    void onRedrawDone();

private :
    MyJkQtPlotter *m_pPlotAScan;
    JKQTPDatastore* m_dsAScan;
    JKQTPXYLineGraph* m_graphAScan;

    size_t m_columnX;
    size_t m_columnY;
    qreal * m_pAScanYData;

    QThread * m_pWorker;
    QTimer * m_timeRefresh;
    qint64 m_lastUpdateEpoc;
    AScanGraphWorker * m_pAScanGraphWorker;
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
};
