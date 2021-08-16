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

// -- Internal includes
#include "AScanGraph.h"
#include <v2/scan/AScan.h>


AScanGraph::AScanGraph(QQuickItem *parent) : QQuickPaintedItem(parent), Loggable("AScanGraph")
{ 
    setFlag( QQuickItem::ItemHasContents, true );
    setAcceptedMouseButtons( Qt::AllButtons );
    m_updateInProgress = false;

    connect( this, &QQuickPaintedItem::widthChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &QQuickPaintedItem::heightChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );

    // -- Make CSCAN
    m_pPlotAScan = new MyJkQtPlotter();
    m_dsAScan = m_pPlotAScan->getDatastore();

    // allocate / init
    QVector<double> xAscan, yAscan;
    for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
        double vy = 0;
        double vx = i * 0.01; // getAScanResolutionUs
        xAscan<<vx;
        yAscan<<vy;
    }

    m_columnX=m_dsAScan->addCopiedColumn(xAscan, "x");
    m_columnY=m_dsAScan->addCopiedColumn(yAscan, "y");

    m_pAScanYData = m_dsAScan->getColumnPointer(m_columnY);

    // -- draw
    m_graphAScan=new JKQTPXYLineGraph(m_pPlotAScan);
    m_graphAScan->setXColumn(m_columnX);
    m_graphAScan->setYColumn(m_columnY);
    m_graphAScan->setSymbolType(JKQTPNoSymbol);
    m_graphAScan->setColor("#202020");
    m_graphAScan->setLineWidth(1.2);
    m_pPlotAScan->addGraph(m_graphAScan);

    // -- comsetic
    m_pPlotAScan->setMousePositionShown(false);
    m_pPlotAScan->setToolbarEnabled(false);
    m_pPlotAScan->clearAllRegisteredMouseDragActions();
    //m_pPlotAScan->setShowZeroAxes(false, false); // hide 0 axis for X and Y (white..)
    m_pPlotAScan->getXAxis()->setShowZeroAxis(false);
    m_pPlotAScan->setContextMenuMode(jkqtpcmmSpecialContextMenu);
    m_pPlotAScan->getXAxis()->setDrawGrid(false);
    m_pPlotAScan->getYAxis()->setDrawGrid(false);
//    m_pPlotAScan->getXAxis()->setAxisLabel("Distance (mm)");
//    m_pPlotAScan->getYAxis()->setAxisLabel("Amplitude (%)");
    m_pPlotAScan->getXAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMnone);
    m_pPlotAScan->getYAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMnone);

    // scale
    m_pPlotAScan->setAbsoluteY(0,100);
    //onDigitilizingDepthChanged();
    m_pPlotAScan->setAbsoluteX(0,40); // X visible area
    m_pPlotAScan->zoom(0, 100, 0, 100);
    m_pPlotAScan->getPlotter()->setBackgroundColor(QColor(255,255,255));
    m_pPlotAScan->getPlotter()->setPlotBorder(-15,10,10,-5);


    // -- Thread
//    m_pWorker = new QThread();
//    this->moveToThread(m_pWorker);

    m_pAScanGraphWorker = new AScanGraphWorker(m_pPlotAScan);
    connect(m_pAScanGraphWorker, &AScanGraphWorker::evRedrawDone, this, &AScanGraph::onRedrawDone, Qt::QueuedConnection);

    //connect(this, &AScanGraph::evPrivateNeedRefresh, this, &AScanGraph::onNeedRefresh);
    //connect(m_pWorker, &QThread::finished, this, &AScanGraph::onThreadFinished);

    // -- Timer
    m_lastUpdateEpoc = 0;
    m_timeRefresh = new QTimer();
    m_timeRefresh->setSingleShot(true);
    //m_timeRefresh->moveToThread(m_pWorker);
    connect(m_timeRefresh, &QTimer::timeout, this, &AScanGraph::onTimerRefresh, Qt::QueuedConnection);

    //m_pWorker->start();
}

AScanGraph::~AScanGraph()
{
//    m_pWorker->quit();
//    m_pWorker->wait(500);
//    if( m_pWorker->isRunning() ) {
//        m_pWorker->terminate();
//    }
//    delete m_pWorker;
//    m_pWorker = nullptr;
    m_pPlotAScan->deleteLater();
    m_pPlotAScan = nullptr;
}

void AScanGraph::paint(QPainter *painter)
{
    //fLogDebug1("!");
    //m_pPlotAScan->redrawPlot();
    if( m_pAScanGraphWorker->waitingUpdate() ) {
        m_updateInProgress = false;
        m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
        m_pPlotAScan->render(painter);
        m_pAScanGraphWorker->notifyUpdate();
    } else {
        // while resizing, use old image to draw something
        QImage* im = m_pPlotAScan->getImage();
        painter->drawImage( QRectF(0,0,width(), height()), *im);
        // request an update to fit well
        m_pAScanGraphWorker->requestRedraw();
    }
}

void AScanGraph::updateAScan(AScan * p_pAScan)
{
    if( nullptr != p_pAScan ) {
        for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
            m_pAScanYData[i] = (double) (p_pAScan->getAScanEnhancedSample()->m_enhancedSamples[i]) * 100 / std::numeric_limits<quint8>::max();
        }
        onNeedRefresh(false);
    } else {
        for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
            m_pAScanYData[i] = 0;
        }
        onNeedRefresh(true);
    }
    //fLogDebug2("data");
}

void AScanGraph::onNeedRefresh(bool p_force)
{
    qint64 curEpoc = QDateTime::currentMSecsSinceEpoch();
    // timer active => already an update programmed
    if( (!m_timeRefresh->isActive() && !m_updateInProgress) || p_force) {
        qint64 dTime = curEpoc - m_lastUpdateEpoc;
        fLogDebug1(QString("refresh dTime = %1").arg(dTime));
        if( dTime >= 50 ) {
            m_updateInProgress = true;
            m_pAScanGraphWorker->requestRedraw();
        } else {
            // start timer with remaining time
            m_timeRefresh->start(50 - (dTime));
        }
    }
    //update();
}

void AScanGraph::onThreadFinished()
{
    if( m_timeRefresh->isActive() ) {
        m_timeRefresh->stop();
    }
    delete m_timeRefresh;
    m_timeRefresh = nullptr;
}

void AScanGraph::onUpdateWidgetSize()
{
    fLogDebug2(QString("width  = %1").arg(width()));
    m_pPlotAScan->setGeometry(0, 0, width(), height());
    onNeedRefresh(false);
}

void AScanGraph::onTimerRefresh()
{
    fLogDebug2("timer");
    onNeedRefresh(false);
}

void AScanGraph::onRedrawDone()
{
    update();
}


// -------------------


AScanGraphWorker::AScanGraphWorker(MyJkQtPlotter *p_pPlotAScan) : Loggable("AScanGraphWorker")
{
    m_pPlotAScan = p_pPlotAScan;
    m_pendingRedraw = false;
    m_processingRedraw = false;
    m_waitingUpdate = false;
    m_pThread = new QThread();

    connect(this, &AScanGraphWorker::evRequestRedraw, this, &AScanGraphWorker::onRequestRedraw, Qt::QueuedConnection);
    //moveToThread(m_pThread);  // OpenGL : Paint device can't be used by multithreading process :-(
    m_pThread->start();

}
AScanGraphWorker::~AScanGraphWorker()
{
    m_pendingRedraw = false;

}

void AScanGraphWorker::requestRedraw()
{
    if( m_processingRedraw ) {
        m_pendingRedraw = true;
    } else {
        m_processingRedraw = true;
        emit evRequestRedraw();
    }
}

void AScanGraphWorker::notifyUpdate()
{
    fLogDebug1("notify");
    m_waitingUpdate = false;
    if( m_pendingRedraw ) {
        m_pendingRedraw = false;
        //m_processingRedraw = false;
        emit evRequestRedraw();
    } else {
        m_processingRedraw = false;
    }
}

bool AScanGraphWorker::waitingUpdate() const
{
    return m_waitingUpdate;
}

bool AScanGraphWorker::isProcessing() const
{
    return m_processingRedraw;
}

bool AScanGraphWorker::isPendingUpdate() const
{
    return m_pendingRedraw;
}

void AScanGraphWorker::onRequestRedraw()
{
    if( m_waitingUpdate ) {
        fLogError("Waiting unpdate !");
    } else {
        //fLogDebug1(">>");
        m_pPlotAScan->redrawPlot();
        //fLogDebug1("<<");
        m_waitingUpdate = true;
        emit evRedrawDone();
    }

}
