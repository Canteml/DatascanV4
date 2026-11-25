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
#include <v2/vector/VectorFuncAScan.h>
#include <scan/ScanManager.h>

#include <jkqtplotter/graphs/jkqtpgeometric.h>

#include <QtGlobal>

#define FREQ_REFRESH_ASCAN_ON_IHM  20

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
    for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_CAPACITY; i++) {
        m_pAScanXData[i] = i;
        m_pAScanYData[i] = 0;

    }

    m_columnX=m_dsAScan->addColumn(m_pAScanXData, AScanRawSample::ASCAN_SAMPLE_CAPACITY/4, "x");
    m_columnY=m_dsAScan->addColumn(m_pAScanYData, AScanRawSample::ASCAN_SAMPLE_CAPACITY/4, "y");
    // RF mode: full resolution columns on the same arrays (a 5 MHz carrier needs all samples to look smooth)
    m_columnXRF=m_dsAScan->addColumn(m_pAScanXData, AScanRawSample::ASCAN_SAMPLE_CAPACITY, "xRF");
    m_columnYRF=m_dsAScan->addColumn(m_pAScanYData, AScanRawSample::ASCAN_SAMPLE_CAPACITY, "yRF");

    // -- Graphic items
    m_pZeroLine = new JKQTPGeoLine(m_pPlotAScan, 0, 0, 0, 100);
    m_pZeroLine->setStyle(QColor("#808080"), 2);
    m_pPlotAScan->addGraph(m_pZeroLine);

    m_graphAScan=new JKQTPXYLineGraph(m_pPlotAScan);
    m_graphAScan->setXColumn(m_columnX);
    m_graphAScan->setYColumn(m_columnY);
    m_graphAScan->setSymbolType(JKQTPNoSymbol);
    m_graphAScan->setColor("#202020");
    m_graphAScan->setLineWidth(0.8);
    m_pPlotAScan->addGraph(m_graphAScan);

    m_graphAScanRF=new JKQTPXYLineGraph(m_pPlotAScan);
    m_graphAScanRF->setXColumn(m_columnXRF);
    m_graphAScanRF->setYColumn(m_columnYRF);
    m_graphAScanRF->setSymbolType(JKQTPNoSymbol);
    m_graphAScanRF->setColor("#202020");
    m_graphAScanRF->setLineWidth(0.8);
    m_graphAScanRF->setVisible(false);
    m_pPlotAScan->addGraph(m_graphAScanRF);
    m_pPlotAScan->getPlotter()->setEmittingPlotSignalsEnabled(false);

    // -- comsetic
    m_pPlotAScan->setMousePositionShown(false);
    m_pPlotAScan->setToolbarEnabled(false);
    m_pPlotAScan->clearAllRegisteredMouseDragActions();
    m_pPlotAScan->setShowZeroAxes(false, false); // hide 0 axis for X and Y (white..)
    m_pPlotAScan->setContextMenuMode(jkqtpcmmSpecialContextMenu);
    m_pPlotAScan->getXAxis()->setDrawGrid(false);
    m_pPlotAScan->getYAxis()->setDrawGrid(false);    
    m_pPlotAScan->getXAxis()->setAutoAxisSpacing(false);
    m_pPlotAScan->getXAxis()->setUserTickSpacing(10);

    m_pPlotAScan->getXAxis()->setLineWidth(1);
    m_pPlotAScan->getYAxis()->setLineWidth(1);
    m_pPlotAScan->getXAxis()->setDrawMode2(JKQTPCADrawMode(JKQTPCADMnone));
    m_pPlotAScan->getYAxis()->setDrawMode2(JKQTPCADrawMode(JKQTPCADMnone));

    // improve performances : CPU : 11% => 5%
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForGraphs(false);
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForSystem(true);
    m_pPlotAScan->getPlotter()->setUseAntiAliasingForText(true);

    // scale
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

void AScanGraph::setAutoGainParams(bool enabled, qint16 threshold, quint16 noiseStart, quint16 noiseWidth){
    m_autoGainEnabled   = enabled;
    m_autoGainThreshold = threshold;
    m_noiseGateStart    = noiseStart;
    m_noiseGateWidth    = noiseWidth;

}

void AScanGraph::setMeasureGate(qreal startMm, qreal widthMm)
{
    m_measureGateStart = startMm;
    m_measureGateWidth = widthMm;
}

int AScanGraph::mmToDisplayIndex(qreal mm, AScan* pAScan, int p_decim) const
{
    qreal stepMm = pAScan->getSensorSettings()->getSampleStepMM();
    if (stepMm <= 0.0) return 0;
    qreal probeDelayMm = pAScan->getAScanMeasure()->m_paramLProbeDelayMm;
    int idx = qRound((mm + probeDelayMm) / (stepMm * p_decim));
    return qBound(0, idx, pAScan->getAScanRawSample()->m_sampleCount / p_decim - 1);
}

AScanGraph::~AScanGraph()
{
    m_mutBuffer.lock();
    if( m_timeRefresh->isActive() ) {
        m_timeRefresh->stop();
    }
    delete m_timeRefresh;
    m_timeRefresh = nullptr;
    delete m_pAScanGraphWorker;
    m_pAScanGraphWorker = nullptr;
    m_mutBuffer.unlock();
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
    if( !m_mouseClicked ) { // Do not update the ASCAN while user drag the graph with the mouse
        if( nullptr != p_pAScan ) {
            qreal d = p_pAScan->getSensorSettings()->getSampleStepMM();
            quint8 wt = p_pAScan->getAScanEnhancedSample()->m_paramDWaveType;
            // Draw a ASCAN with 4000 points use lot of CPU.
            // Reduce to 1000 points (/4), except in RF mode: averaging 4 samples
            // leaves only 4 points per period of a 5 MHz carrier (80 MHz) => angular curve.
            // the progressive smoothing is then done by the RF low-pass filter (AScanWorker)
            static const int RF_LISSAGE_DISPLAY_FULL = 5;   ///< above: 4000 points
            const bool isRF = (wt == 3);
            const bool fullRes = isRF && p_pAScan->getAScanEnhancedSample()->m_paramLLowPassFilter > RF_LISSAGE_DISPLAY_FULL;
            const int decim = fullRes ? 1 : 4;
            const int N = p_pAScan->getAScanRawSample()->m_sampleCount / decim;
            m_graphAScan->setVisible(!fullRes);
            m_graphAScanRF->setVisible(fullRes);
            // Anti-aliasing costs CPU: only when the RF curve is drawn at higher resolution
            m_pPlotAScan->getPlotter()->setUseAntiAliasingForGraphs(decim < 4);

            for(int i=0; i<N; i++) {
                int is = i*decim;
                qreal x = d*is - p_pAScan->getAScanMeasure()->m_paramLProbeDelayMm;
                m_pAScanXData[i] = x;
                qreal ymoy = 0;
                for(int j=is; j<is+decim; j++) {
                    ymoy += p_pAScan->getAScanEnhancedSample()->m_enhancedSamples[j] / qreal(AScanEnhancedSample::ENHANCED_SCALE);
                }
                if (isRF) {
                    m_pAScanYData[i] = (ymoy/decim - 127.0) / std::numeric_limits<quint8>::max() * 100.0; // RF: centrato su 0, range -50..+50
                } else {
                    m_pAScanYData[i] = (ymoy/decim) * 100.0 / std::numeric_limits<quint8>::max();
                }
            }

            if (m_autoGainEnabled) {
                int noiseStart = mmToDisplayIndex(static_cast<qreal>(m_noiseGateStart),                    p_pAScan, decim);
                int noiseEnd   = mmToDisplayIndex(static_cast<qreal>(m_noiseGateStart + m_noiseGateWidth), p_pAScan, decim);

                int measStart  = mmToDisplayIndex(m_measureGateStart,                          p_pAScan, decim);
                int measEnd    = mmToDisplayIndex(m_measureGateStart + m_measureGateWidth,     p_pAScan, decim);

                if (wt == 3) {
                    // RF AutoGain: normalizza il picco assoluto del gate di misura all'80% del range
                    qreal scaleFactor = 1.0;
                    qreal measAbsMax = 0.0;
                    for (int i = measStart; i <= measEnd; i++) {
                        qreal absY = qAbs(m_pAScanYData[i]);
                        if (absY > measAbsMax) measAbsMax = absY;
                    }
                    if (measAbsMax > 1.0)
                        scaleFactor = 40.0 / measAbsMax; // 80% di ±50

                    // Noise gate RF: threshold >= 0 → controlla massimo; threshold < 0 → controlla minimo
                    const qreal thr = static_cast<qreal>(m_autoGainThreshold);
                    if (thr >= 0.0) {
                        for (int i = noiseStart; i <= noiseEnd; i++) {
                            if (m_pAScanYData[i] > thr) { scaleFactor = 1.0; break; }
                        }
                    } else {
                        for (int i = noiseStart; i <= noiseEnd; i++) {
                            if (m_pAScanYData[i] < thr) { scaleFactor = 1.0; break; }
                        }
                    }

                    for (int i = 0; i < N; i++) {
                        m_pAScanYData[i] = qBound<qreal>(-50.0, m_pAScanYData[i] * scaleFactor, 50.0);
                    }
                } else {
                    // 1. Calcola scaleFactor dal picco nel gate di misura
                    qreal scaleFactor = 1.0;
                    qreal measMax = 0.0;
                    for (int i = measStart; i <= measEnd; i++) {
                        if (m_pAScanYData[i] > measMax) measMax = m_pAScanYData[i];
                    }
                    if (measMax > 1.0) {
                        scaleFactor = 80.0 / measMax;
                    }

                    // 2. Controlla il rumore sul segnale amplificato
                    qreal noiseMax = 0.0;
                    for (int i = noiseStart; i <= noiseEnd; i++) {
                        if (m_pAScanYData[i] > noiseMax) noiseMax = m_pAScanYData[i];
                    }
                    if (noiseMax * scaleFactor > static_cast<qreal>(m_autoGainThreshold)) {
                        scaleFactor = 1.0; // Rumore eccessivo: mostra segnale originale
                    }

                    // Applica
                    for (int i = 0; i < N; i++) {
                        m_pAScanYData[i] = qBound<qreal>(0.0, m_pAScanYData[i] * scaleFactor, 100.0);
                    }
                }
            }

            // Le colonne hanno lunghezza fissa : i punti in eccesso ripetono l'ultimo campione
            const int columnSize = AScanRawSample::ASCAN_SAMPLE_CAPACITY / decim;
            for(int i=N; i<columnSize; i++) {
                m_pAScanXData[i] = m_pAScanXData[N-1];
                m_pAScanYData[i] = m_pAScanYData[N-1];
            }

        } else {
            for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_CAPACITY; i++) {
                m_pAScanYData[i] = 0;
            }
        }

        needRefresh(false);
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

    // RF mode (paramDWaveType == 3) changes the Y range: re-apply the settings
    // whenever the wave type of the selected A-scan function changes.
    auto reconnectWaveType = [this](VectorFuncAScan* func) {
        disconnect(m_waveTypeConnection);
        if (func) {
            m_waveTypeConnection = connect(func, &VectorFuncAScan::paramDWaveTypeChanged,
                                           this, [this]{ customApplySettings(); });
            customApplySettings();
        }
    };
    connect(ScanManager::getInstance(), &ScanManager::evSelectedFuncAScanChanged,
            this, reconnectWaveType);
    reconnectWaveType(ScanManager::getInstance()->getSelectedFuncAScan());
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
    m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax, m_AxeYMin, m_AxeYMax); // X visible area
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
    m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax, m_AxeYMin, m_AxeYMax); // X visible area
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
        if( (dTime >= (1000/FREQ_REFRESH_ASCAN_ON_IHM) || p_force) && !m_updateInProgress ) {
            m_updateInProgress = true;
            m_lastUpdateEpoc = curEpoc;
            if(  nullptr != m_pAScanGraphWorker ) {
                m_pAScanGraphWorker->requestUpdate();
            }
        } else if( !m_timeRefresh->isActive() ){
            // start timer with remaining time
            m_timeRefresh->start(qMax<qint64>(5, (1000/FREQ_REFRESH_ASCAN_ON_IHM) - (dTime)));
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QWheelEvent* newEvent = new QWheelEvent(
            event->position(),          // pos (local)
            event->globalPosition(),    // globalPos
            event->pixelDelta(),        // pixelDelta
            event->angleDelta(),        // angleDelta
            event->buttons(),           // buttons
            event->modifiers(),         // modifiers
            event->phase(),             // phase
            event->isInverted(),        // inverted
            event->source()             // source
            );
#else
        QWheelEvent* newEvent = new QWheelEvent(*event);
#endif
        QCoreApplication::postEvent( m_pPlotAScan, newEvent );
    }
}

void AScanGraph::hoverMoveEvent(QHoverEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointF pos = event->position();
    if( nullptr != m_eMouseMove && pos != m_eMouseMove->position() ) {
        delete m_eMouseMove;
        m_eMouseMove = new QMouseEvent(QEvent::MouseMove, pos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        routeMouseEvents( m_eMouseMove );
    }
#else
    if( nullptr != m_eMouseMove && event->posF() != m_eMouseMove->localPos() ) {
        m_eMouseMove->setLocalPos(event->posF());
        routeMouseEvents( m_eMouseMove );
    }
#endif
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

    VectorFuncAScan* mode = ScanManager::getInstance()->getSelectedFuncAScan();
    bool isRF = mode && (mode->getParamDWaveType() == 3);
    m_AxeYMin = isRF ? -50.0 : 0.0;
    m_AxeYMax = isRF ? 50.0 : 100.0;

    if (m_pZeroLine) {
        m_pZeroLine->setY1(m_AxeYMin);
        m_pZeroLine->setY2(m_AxeYMax);
    }

    if( nullptr != m_pPlotAScan ) {
        m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax, m_AxeYMin, m_AxeYMax); // X visible area
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

        m_pPlotAScan->zoom(m_AxeXMin, m_AxeXMax, m_AxeYMin, m_AxeYMax); // X visible area
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
    // Do NOT move the worker to m_pThread: draw() renders the JKQTPlotter, which is
    // not thread-safe (JKQTMathText::parse() creates QObject children of a GUI-thread
    // object on every tick label, and the GUI thread modifies the plot concurrently)
    // => "Cannot create children for a parent that is in a different thread" + SIGSEGV.
    // The queued connection below keeps the update asynchronous, on the GUI thread (as BScanGraph).

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

