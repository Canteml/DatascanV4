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

#include <jkqtplotter/graphs/jkqtpgeometric.h>

#include <QtGlobal>

AScanGraph::AScanGraph(QQuickItem *parent) : QQuickPaintedItem(parent), XmlSettingsLoggable("AScanGraph")
{ 
    setAcceptHoverEvents(true);
    setFlag( QQuickItem::ItemHasContents, true );
    setFlag( QQuickItem::ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons( Qt::AllButtons );
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    m_updateInProgress = false;
    m_mouseClicked = false;
    m_mouseClickPressX = 0;
    m_AxeXMin = 0;
    m_AxeXMax = 100;
    m_curBuffer = 0;

    // Worker

    m_pAScanGraphWorker = new AScanGraphWorker(this);
    connect(m_pAScanGraphWorker, &AScanGraphWorker::evUpdateDone, this, &AScanGraph::onAScanReplotted, Qt::QueuedConnection);

    m_eMouseMove = new QMouseEvent(QEvent::MouseMove, QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);

    connect( this, &QQuickPaintedItem::widthChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &QQuickPaintedItem::heightChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &AScanGraph::widthChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &AScanGraph::heightChanged, this, &AScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );

    // -- Make CSCAN
    m_pPlotAScan = new MyJkQtPlotter();
    m_dsAScan = m_pPlotAScan->getDatastore();

    // allocate / init
    //QVector<double> xAscan, yAscan;
    for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
//        double vy = 0;
//        double vx = i;
//        xAscan<<vx;
//        yAscan<<vy;
        m_pAScanXData[i] = i;
        m_pAScanYData[i] = 0;

    }

//    m_columnX=m_dsAScan->addCopiedColumn(xAscan, "x");
//    m_columnY=m_dsAScan->addCopiedColumn(yAscan, "y");

    m_columnX=m_dsAScan->addColumn(m_pAScanXData, AScanRawSample::ASCAN_SAMPLE_SIZE, "x");
    m_columnY=m_dsAScan->addColumn(m_pAScanYData, AScanRawSample::ASCAN_SAMPLE_SIZE, "y");

//    m_pAScanXData = m_dsAScan->getColumnPointer(m_columnX);
//    m_pAScanYData = m_dsAScan->getColumnPointer(m_columnY);

    // -- Graphic items
    m_pZeroLine = new JKQTPGeoLine(m_pPlotAScan, 0, 0, 0, 100);
    m_pZeroLine->setStyle(QColor("#808080"), 2);
    m_pPlotAScan->addGraph(m_pZeroLine);

    m_graphAScan=new JKQTPXYLineGraph(m_pPlotAScan);
    m_graphAScan->setXColumn(m_columnX);
    m_graphAScan->setYColumn(m_columnY);
    m_graphAScan->setSymbolType(JKQTPNoSymbol);
    m_graphAScan->setColor("#202020");
    m_graphAScan->setLineWidth(0.8); //(1.2);
    m_pPlotAScan->addGraph(m_graphAScan);
    m_pPlotAScan->getPlotter()->setEmittingPlotSignalsEnabled(false);

    // -- comsetic
    m_pPlotAScan->setMousePositionShown(false);
    m_pPlotAScan->setToolbarEnabled(false);
    m_pPlotAScan->clearAllRegisteredMouseDragActions();
    m_pPlotAScan->setShowZeroAxes(false, false); // hide 0 axis for X and Y (white..)
    //m_pPlotAScan->getXAxis()->setShowZeroAxis(false);
    m_pPlotAScan->setContextMenuMode(jkqtpcmmSpecialContextMenu);
    m_pPlotAScan->getXAxis()->setDrawGrid(false);
    m_pPlotAScan->getYAxis()->setDrawGrid(false);    
    //m_pPlotAScan->getXAxis()->setMinTicks(10);
    m_pPlotAScan->getXAxis()->setAutoAxisSpacing(false);
    m_pPlotAScan->getXAxis()->setUserTickSpacing(10);

    m_pPlotAScan->getXAxis()->setLineWidth(1);
    m_pPlotAScan->getYAxis()->setLineWidth(1);
//    m_pPlotAScan->getXAxis()->setAxisLabel("Distance (mm)");
//    m_pPlotAScan->getYAxis()->setAxisLabel("Amplitude (%)");
    m_pPlotAScan->getXAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMnone);
    m_pPlotAScan->getYAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMnone);

    // improve performances : CPU : 11% => 5%
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForGraphs(false);
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForSystem(false);
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForText(true);

    // scale
    //m_pPlotAScan->setAbsoluteY(0,100);
    //onDigitilizingDepthChanged();
    //m_pPlotAScan->setAbsoluteX(-1000,1000); // X visible area
    //m_pPlotAScan->getXAxis()->setRange(0,20);
    m_pPlotAScan->zoom(0, 100, 0, 100);
    m_pPlotAScan->getPlotter()->setBackgroundColor(QColor(255,255,255));
    m_pPlotAScan->getPlotter()->setPlotBorder(-15,10,10,-5);

    connect(m_pPlotAScan, &MyJkQtPlotter::evRequestUpdate, this, &AScanGraph::onPrivateNeedRefresh, Qt::QueuedConnection);
    connect(this, &AScanGraph::evPrivateNeedRefresh, this, &AScanGraph::onPrivateNeedRefresh);

    // -- Timer
    m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
    m_timeRefresh = new QTimer();
    m_timeRefresh->setSingleShot(true);
    connect(m_timeRefresh, &QTimer::timeout, this, &AScanGraph::onTimerRefresh, Qt::QueuedConnection);

}

AScanGraph::~AScanGraph()
{
    delete m_pAScanGraphWorker;
    m_pAScanGraphWorker = nullptr;
    m_pPlotAScan->deleteLater();
    m_pPlotAScan = nullptr;
    delete m_eMouseMove;
    m_eMouseMove = nullptr;
}

void AScanGraph::paint(QPainter *painter)
{
    m_mutBuffer.lock();
    QImage * buffer = &m_buffer[m_curBuffer];
    painter->drawImage(QRect(0,0, width(), height()),*buffer);
    m_mutBuffer.unlock();
    m_updateInProgress = false;
    if( nullptr != m_pAScanGraphWorker ) {
        m_pAScanGraphWorker->signalPaintDone();
    }
}

void AScanGraph::stop()
{
    m_pAScanGraphWorker->stop();

}

void AScanGraph::updateAScan(AScan * p_pAScan)
{
    if( !m_mouseClicked ) {
        if( nullptr != p_pAScan ) {
            qreal d = p_pAScan->getSensorSettings()->getSampleStepMM();
            for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
                qreal x = d*i-p_pAScan->getAScanMeasure()->m_paramLProbeDelayMm;
                m_pAScanXData[i] = x;
                m_pAScanYData[i] = (double) (p_pAScan->getAScanEnhancedSample()->m_enhancedSamples[i]) * 100 / std::numeric_limits<quint8>::max();
            }
            needRefresh(false);
        } else {
            for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
                m_pAScanYData[i] = 0;
            }
            needRefresh(false);
        }
    }
}

MyJkQtPlotter *AScanGraph::getPlot() const
{
    return m_pPlotAScan;
}

void AScanGraph::initAfterItemAdded()
{
    // -- mouse
    connect(m_pPlotAScan, SIGNAL(plotMouseMove(double, double)), this, SLOT(onPlotMouseMove(double, double)));
    connect(m_pPlotAScan, SIGNAL(plotMouseClicked(double, double, Qt::KeyboardModifiers , Qt::MouseButton)), this, SLOT(onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)));

    m_pAScanGraphWorker->start();

    needRefresh(false);
}

void AScanGraph::zoomIn()
{
    qreal d = m_AxeXMax - m_AxeXMin;
    if( d<10 && d>4 ) {
        m_AxeXMax -= 1;
    } else if( d < 20 ) {
        m_AxeXMax -= 2;
    } else if( d < 50 ) {
        m_AxeXMax -= 5;
    } else if( d < 100 ) {
        m_AxeXMax -= 10;
    } else if( d < 200 ) {
        m_AxeXMax -= 20;
    }  else {
        m_AxeXMax -= 50;
    }

    computeTickSpacing();
    m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax,0 ,100); // X visible area
    needRefresh(true);
}

void AScanGraph::zoomOut()
{
    qreal d = m_AxeXMax - m_AxeXMin;
    if( d<10 ) {
        m_AxeXMax += 1;
    } else if( d < 20 ) {
        m_AxeXMax += 2;
    } else if( d < 50 ) {
        m_AxeXMax += 5;
    } else if( d < 100 ) {
        m_AxeXMax += 10;
    } else if( d < 200 ) {
        m_AxeXMax += 20;
    }else {
        m_AxeXMax += 50;
    }

    computeTickSpacing();
    m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax,0 ,100); // X visible area
    needRefresh(true);
}


void AScanGraph::computeTickSpacing()
{
    qreal d = m_AxeXMax - m_AxeXMin;
    if( d<=50 ) {
        m_pPlotAScan->getXAxis()->setUserTickSpacing(5);
    } else if( d <= 150 ) {
        m_pPlotAScan->getXAxis()->setUserTickSpacing(10);
    } else if( d <= 250 ) {
        m_pPlotAScan->getXAxis()->setUserTickSpacing(20);
    }else {
        m_pPlotAScan->getXAxis()->setUserTickSpacing(50);
    }
}

void AScanGraph::routeMouseEvents(QMouseEvent *event)
{
    if( nullptr != m_pPlotAScan ) {
        QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        QCoreApplication::postEvent( m_pPlotAScan, newEvent );
    }
}

void AScanGraph::needRefresh(bool p_force)
{
    qint64 curEpoc = QDateTime::currentMSecsSinceEpoch();
    // timer active => already an update programmed
    if( !m_timeRefresh->isActive() || p_force) {
        qint64 dTime = curEpoc - m_lastUpdateEpoc;
        if( dTime >= 60 || p_force ) {
            m_updateInProgress = true;
            m_lastUpdateEpoc = curEpoc;
            //update();
            if(  nullptr != m_pAScanGraphWorker ) {
                m_pAScanGraphWorker->requestUpdate();
            }
        } else if( !m_timeRefresh->isActive() ){
            // start timer with remaining time
            m_timeRefresh->start(60 - (dTime));
        }
    }
}

void AScanGraph::draw()
{
    QImage * buffer = &(m_buffer[1-m_curBuffer]);
    if( (buffer->width() != width() || buffer->height() !=  height() ) && (width() !=0 && height() != 0) ) {
        (*buffer) = QImage(width(), height(), QImage::Format_RGB32);
    }
    qreal magnification = m_pPlotAScan->getMagnification();
    m_pPlotAScan->getPlotter()->setWidgetSize(jkqtp_roundTo<int>(width()/magnification), jkqtp_roundTo<int>(height()/magnification));
    JKQTPEnhancedPainter t(buffer);
    t.scale(magnification, magnification);
    m_pPlotAScan->getPlotter()->draw(t, QRect(0,0,width(), height()));
}


void AScanGraph::mousePressEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void AScanGraph::mouseReleaseEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
    if( m_mouseClicked && event->button() == Qt::LeftButton) {
        m_mouseClicked = false;
        m_pPlotAScan->setMouseUsedByItem(false);

    }
}

void AScanGraph::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );    
}

void AScanGraph::mouseDoubleClickEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void AScanGraph::wheelEvent(QWheelEvent *event)
{
    if( nullptr != m_pPlotAScan ) {
        QWheelEvent* newEvent = new QWheelEvent(*event);
        //QCoreApplication::sendEvent( m_CustomPlot, newEvent );
        QCoreApplication::postEvent( m_pPlotAScan, newEvent );
    }
}

void AScanGraph::hoverMoveEvent(QHoverEvent *event)
{
    if( nullptr != m_eMouseMove && event->posF() != m_eMouseMove->localPos() ) {
        m_eMouseMove->setLocalPos(event->posF());
        routeMouseEvents( m_eMouseMove );
    }
}

bool AScanGraph::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_AxeXMin, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_AxeXMax, 100);

    return bRet;
}

bool AScanGraph::customLoadDefaultSettings()
{
    bool bRet = true;
    m_AxeXMin = 0;
    m_AxeXMax = 100;
    return bRet;
}

bool AScanGraph::customApplySettings()
{
    bool bRet = true;
    computeTickSpacing();
    if( nullptr != m_pPlotAScan ) {
        m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax,0 ,100); // X visible area
    }
    return bRet;
}

bool AScanGraph::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_AxeXMin);
    bRet &= XML_WRITE_UNIQUE(m_AxeXMax);
    return bRet;
}

void AScanGraph::onPrivateNeedRefresh()
{
    needRefresh(false);
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
    // update replot time to get a delay
    //m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
    needRefresh(false);
}

void AScanGraph::onTimerRefresh()
{
    needRefresh(false);
}

void AScanGraph::onAScanReplotted()
{
    update();
}

void AScanGraph::onPlotMouseMove(double x, double y)
{
    m_mouseLastPos = QPointF(x,y);

    if( m_mouseClicked ) {
        // drag on the graph
        qreal dX = m_mouseClickPressX - m_mouseLastPos.x();
        qreal newXMin = m_AxeXMin+dX;
        qreal newXMax = m_AxeXMax+dX;
        if( newXMax > m_pPlotAScan->getAbsoluteXMax() ) {
            newXMax = m_pPlotAScan->getAbsoluteXMax();
            newXMin = newXMax - (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }
        if( newXMin < m_pPlotAScan->getAbsoluteXMin() ) {
            newXMin = m_pPlotAScan->getAbsoluteXMin();
            newXMax = newXMin + (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }
        m_AxeXMin = newXMin;
        m_AxeXMax = newXMax;

        m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax,0 ,100); // X visible area
        needRefresh(true);
    }
}

void AScanGraph::onPlotMouseClicked(double x, double y, Qt::KeyboardModifiers, Qt::MouseButton p_Bt)
{
    m_mouseLastPos = QPointF(x,y);

    if( !m_pPlotAScan->getMouseUsedByItem() && p_Bt == Qt::LeftButton ) {
        // prepare drag : store where mouse has clicked
        m_pPlotAScan->setMouseUsedByItem(true);
        m_mouseClicked = true;
        m_mouseClickPressX = m_mouseLastPos.x();
        m_mouseClickPressAxeXMin = m_AxeXMin;
        m_mouseClickPressAxeXMax = m_AxeXMax;
    }
}


// ----------------------------

AScanGraphWorker::AScanGraphWorker(AScanGraph * p_parent) : Loggable("AScanGraphWorker")
{
    m_pParent = p_parent;
    m_requestUpdate = false;
    m_processing = false;

    m_pThread = new QThread();
    this->moveToThread(m_pThread);

    connect(this, &AScanGraphWorker::evRequestUpdate, this, &AScanGraphWorker::onProcessUpdate, Qt::QueuedConnection);
}

AScanGraphWorker::~AScanGraphWorker()
{
    if(m_pThread->isRunning()) {
        m_pThread->quit();
        m_pThread->wait(500);
    }
    delete m_pThread;
    m_pThread = nullptr;
}

void AScanGraphWorker::requestUpdate()
{
    if( m_processing ) {
        m_requestUpdate = true;
    } else {
        m_processing = true;
        m_requestUpdate = false;
        emit evRequestUpdate();
    }
}

void AScanGraphWorker::signalPaintDone()
{
    if( m_requestUpdate ) {
        m_requestUpdate = false;
        emit evRequestUpdate();
    } else {
        m_processing = false;
    }
}

void AScanGraphWorker::start()
{
    m_pThread->start();
}

void AScanGraphWorker::stop()
{
    disconnect(this, &AScanGraphWorker::evRequestUpdate, this, &AScanGraphWorker::onProcessUpdate);
    m_pThread->quit();
    m_pThread->wait(500);
}

void AScanGraphWorker::onProcessUpdate()
{
    m_pParent->draw();
    m_pParent->m_mutBuffer.lock();
    m_pParent->m_curBuffer = 1-m_pParent->m_curBuffer;
    m_pParent->m_mutBuffer.unlock();
    emit evUpdateDone();
}

