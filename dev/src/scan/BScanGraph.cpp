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

// -- Internal includes
#include "BScanGraph.h"
#include <v2/scan/Surface1d.h>

#include <jkqtplotter/graphs/jkqtpgeometric.h>

#include <QtGlobal>

#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncPosition.h>
#include "exam/ExamManager.h"

BScanGraph::BScanGraph(QQuickItem *parent) : QQuickPaintedItem(parent), XmlSettingsLoggable("BScanGraph")
{ 
    setAcceptHoverEvents(true);
    setFlag( QQuickItem::ItemHasContents, true );
    setFlag( QQuickItem::ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons( Qt::AllButtons );
    m_updateInProgress = false;
    m_mouseClicked = false;
    m_mouseClickPressX = 0;
    m_AxeXScaleM = 2;
    m_AxeYMaxMm = 20;
    m_zoomXMode = ZOOM_X_AUTO;

    m_eMouseMove = new QMouseEvent(QEvent::MouseMove, QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);

    connect( this, &QQuickPaintedItem::widthChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &QQuickPaintedItem::heightChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &BScanGraph::widthChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &BScanGraph::heightChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );

    connect( VectorManager::getInstance()->getVectorStd()->funcPosition(), &VectorFuncPosition::paramLBscanPosMmXChanged, this, &BScanGraph::onPositionChanged);

    // -- Make BSCAN
    m_pPlotBScan = new MyJkQtPlotter();
    m_dsBScan = m_pPlotBScan->getDatastore();

//    // allocate / init
//    QVector<double> xAscan, yAscan;
//    for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_SIZE; i++) {
//        double vy = 0;
//        double vx = i * 0.01; // getAScanResolutionUs
//        xAscan<<vx;
//        yAscan<<vy;
//    }

//    m_columnX=m_dsBScan->addCopiedColumn(xAscan, "x");
//    m_columnY=m_dsBScan->addCopiedColumn(yAscan, "y");

//    m_pAScanXData = m_dsBScan->getColumnPointer(m_columnX);
//    m_pAScanYData = m_dsBScan->getColumnPointer(m_columnY);

    // -- Graphic items

    m_graphBScan=new JKQTPImpulsesVerticalGraph(m_pPlotBScan);
    m_graphBScan->setXColumn(m_columnX);
    m_graphBScan->setYColumn(m_columnY);
    m_graphBScan->setSymbolType(JKQTPNoSymbol);
    m_graphBScan->setColor("#80202020");
    m_graphBScan->setLineWidth(5);
    m_pPlotBScan->addGraph(m_graphBScan);

    m_pPositionLine = new JKQTPGeoLine(m_pPlotBScan, 0, 0, 0, 100);
    m_pPositionLine->setStyle(QColor("#40DD40"), 3);
    m_pPlotBScan->addGraph(m_pPositionLine);

    // -- comsetic
    m_pPlotBScan->setMousePositionShown(false);
    m_pPlotBScan->setToolbarEnabled(false);
    m_pPlotBScan->clearAllRegisteredMouseDragActions();
    m_pPlotBScan->setShowZeroAxes(false, false); // hide 0 axis for X and Y (white..)
    //m_pPlotAScan->getXAxis()->setShowZeroAxis(false);
    m_pPlotBScan->setContextMenuMode(jkqtpcmmSpecialContextMenu);
    m_pPlotBScan->getXAxis()->setDrawGrid(false);
    m_pPlotBScan->getYAxis()->setDrawGrid(true);
    m_pPlotBScan->getXAxis()->setLineWidth(1.1);
    m_pPlotBScan->getYAxis()->setLineWidth(1);
//    m_pPlotAScan->getXAxis()->setAxisLabel("Distance (mm)");
//    m_pPlotAScan->getYAxis()->setAxisLabel("Amplitude (%)");
    m_pPlotBScan->getXAxis()->setDrawMode1(JKQTPCADrawMode::JKQTPCADMLineTicks);
    m_pPlotBScan->getXAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMLineTicksTickLabels);

    //m_pPlotBScan->getYAxis()->setDrawMode1(JKQTPCADrawMode::JKQTPCADMnone);
    m_pPlotBScan->getYAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMLineTicksTickLabels );

    m_pPlotBScan->getYAxis()->setInverted(true);

    // improve performances
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForGraphs(true);
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForSystem(true);
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForText(true);

    // scale
    //m_pPlotAScan->setAbsoluteY(0,100);
    //onDigitilizingDepthChanged();
    //m_pPlotAScan->setAbsoluteX(-1000,1000); // X visible area
    //m_pPlotAScan->getXAxis()->setRange(0,20);
    m_pPlotBScan->zoom(0, 2, 0, 20);
    m_pPlotBScan->getPlotter()->setBackgroundColor(QColor(255,255,255));
    m_pPlotBScan->getPlotter()->setPlotBorder(-15,5,-5,5);

    connect(m_pPlotBScan, &MyJkQtPlotter::evRequestUpdate, this, &BScanGraph::onPrivateNeedRefresh, Qt::QueuedConnection);
    connect(this, &BScanGraph::evPrivateNeedRefresh, this, &BScanGraph::onPrivateNeedRefresh);

    // -- Timer
    m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
    m_timeRefresh = new QTimer();
    m_timeRefresh->setSingleShot(true);
    connect(m_timeRefresh, &QTimer::timeout, this, &BScanGraph::onTimerRefresh, Qt::QueuedConnection);

}

BScanGraph::~BScanGraph()
{
    m_pPlotBScan->deleteLater();
    m_pPlotBScan = nullptr;
    delete m_eMouseMove;
    m_eMouseMove = nullptr;
}

void BScanGraph::paint(QPainter *painter)
{
    // need resize ?
    if( m_pPlotBScan->width() != width() || m_pPlotBScan->height() != height() ) {
        m_pPlotBScan->resize(width(), height());
    }
    // update replot time
    m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
    // process replot
    m_pPlotBScan->redrawPlot();
    m_pPlotBScan->render(painter);
    m_updateInProgress = false;
}

void BScanGraph::onUpdateBScan(Surface1d* p_surface)
{
    if( !m_mouseClicked ) {
        if( nullptr != p_surface ) {
            if( p_surface->getResolution() != m_graphBScan->getLineWidth()/2 ) {
                m_graphBScan->setLineWidth(p_surface->getResolution()/2);
            }
            qint32 nbCell =  p_surface->getNbCell();
            if( 0 != nbCell ) {
                m_columnX=m_dsBScan->addColumn(p_surface->getArrayPosition()->data(), nbCell, "x");
                m_columnY=m_dsBScan->addColumn(p_surface->getArrayMeasure()->data(), nbCell, "y");
                m_graphBScan->setXColumn(m_columnX);
                m_graphBScan->setYColumn(m_columnY);
                needRefresh(false);
            }
        } else {
            m_pPlotBScan->clearGraphs(false);
            needRefresh(false);
        }
    }
}

MyJkQtPlotter *BScanGraph::getPlot() const
{
    return m_pPlotBScan;
}

void BScanGraph::initAfterItemAdded()
{
    // -- mouse
    connect(m_pPlotBScan, SIGNAL(plotMouseMove(double, double)), this, SLOT(onPlotMouseMove(double, double)));
    connect(m_pPlotBScan, SIGNAL(plotMouseClicked(double, double, Qt::KeyboardModifiers , Qt::MouseButton)), this, SLOT(onPlotMouseClicked(double, double, Qt::KeyboardModifiers, Qt::MouseButton)));

    m_lastPositionM = VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLBscanPosMmX();
    connect(ExamManager::getInstance(), &ExamManager::evRequestSurfaceUpdate, this, &BScanGraph::onUpdateBScan);
}

void BScanGraph::scaleXIn()
{
    qreal old = m_AxeXScaleM;
    if( m_AxeXScaleM <= 4 && m_AxeXScaleM>1 ) {
        m_AxeXScaleM -= 1;
    } else if( m_AxeXScaleM <= 10 ) {
        m_AxeXScaleM -= 2;
    } else if( m_AxeXScaleM <= 30 ) {
        m_AxeXScaleM -= 5;
    } else if( m_AxeXScaleM <= 100 ) {
        m_AxeXScaleM -= 10;
    }
    if( old != m_AxeXScaleM ) {
        updateAxes();
    }
}

void BScanGraph::scaleXOut()
{
    qreal old = m_AxeXScaleM;
    if( m_AxeXScaleM < 4 && m_AxeXScaleM>1 ) {
        m_AxeXScaleM += 1;
    } else if( m_AxeXScaleM < 10 ) {
        m_AxeXScaleM += 2;
    } else if( m_AxeXScaleM < 30 ) {
        m_AxeXScaleM += 5;
    } else if( m_AxeXScaleM < 100 ) {
        m_AxeXScaleM += 10;
    }
    if( old != m_AxeXScaleM ) {
        updateAxes();
    }
}

void BScanGraph::thicknessYIn()
{
    qreal old = m_AxeYMaxMm;
    if( m_AxeYMaxMm <= 10 && m_AxeYMaxMm>3 ) {
        m_AxeYMaxMm -= 1;
    } else if( m_AxeYMaxMm <= 20 ) {
        m_AxeYMaxMm -= 2;
    } else if( m_AxeYMaxMm <= 30 ) {
        m_AxeYMaxMm -= 5;
    } else if( m_AxeYMaxMm <= 100 ) {
        m_AxeYMaxMm -= 10;
    }
    if( old != m_AxeYMaxMm ) {
        updateAxes();
    }
}

void BScanGraph::thicknessYOut()
{
    qreal old = m_AxeYMaxMm;
    if( m_AxeYMaxMm < 10 && m_AxeYMaxMm>3 ) {
        m_AxeYMaxMm += 1;
    } else if( m_AxeYMaxMm < 20 ) {
        m_AxeYMaxMm += 2;
    } else if( m_AxeYMaxMm < 30 ) {
        m_AxeYMaxMm += 5;
    } else if( m_AxeYMaxMm < 100 ) {
        m_AxeYMaxMm += 10;
    }
    if( old != m_AxeYMaxMm ) {
        updateAxes();
    }
}

void BScanGraph::setZoomXMode(qint32 p_value)
{
    if( m_zoomXMode != p_value ) {
        m_zoomXMode = p_value;
        emit evZoomXModeChanged(m_zoomXMode);
    }
}

qint32 BScanGraph::getZoomXMode() const
{
    return m_zoomXMode;
}

void BScanGraph::routeMouseEvents(QMouseEvent *event)
{
    if( nullptr != m_pPlotBScan ) {
        QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        QCoreApplication::postEvent( m_pPlotBScan, newEvent );
    }
}

void BScanGraph::needRefresh(bool p_force)
{
    qint64 curEpoc = QDateTime::currentMSecsSinceEpoch();
    // timer active => already an update programmed
    if( (!m_timeRefresh->isActive() && !m_updateInProgress) || p_force) {
        qint64 dTime = curEpoc - m_lastUpdateEpoc;
        if( dTime >= 40 || p_force ) {
            m_updateInProgress = true;
            update();
        } else if( !m_timeRefresh->isActive() ){
            // start timer with remaining time
            m_timeRefresh->start(40 - (dTime));
        }
    }
}


void BScanGraph::mousePressEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void BScanGraph::mouseReleaseEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
    if( m_mouseClicked && event->button() == Qt::LeftButton) {
        m_mouseClicked = false;
        m_pPlotBScan->setMouseUsedByItem(false);
    }
}

void BScanGraph::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );    
}

void BScanGraph::mouseDoubleClickEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void BScanGraph::wheelEvent(QWheelEvent *event)
{
    if( nullptr != m_pPlotBScan ) {
        QWheelEvent* newEvent = new QWheelEvent(*event);
        //QCoreApplication::sendEvent( m_CustomPlot, newEvent );
        QCoreApplication::postEvent( m_pPlotBScan, newEvent );
    }
}

void BScanGraph::hoverMoveEvent(QHoverEvent *event)
{
    if( nullptr != m_eMouseMove && event->posF() != m_eMouseMove->localPos() ) {
        m_eMouseMove->setLocalPos(event->posF());
        routeMouseEvents( m_eMouseMove );
    }
}

void BScanGraph::onPositionChanged(qreal p_value)
{
    m_lastPositionM = p_value/1000;

    qreal xMin = m_pPlotBScan->getXMin();
    qreal xMax = m_pPlotBScan->getXMax();
    qreal d = abs(xMax - xMin);
    qreal v = d*0.2;
    if( m_lastPositionM < (xMin+v) || m_lastPositionM < (xMax-v) ) {
        updateAxes();
    }

    // update graph
    m_pPositionLine->setX1(m_lastPositionM);
    m_pPositionLine->setX2(m_lastPositionM);

    needRefresh(false);
}

void BScanGraph::updateAxes()
{
    if( ZOOM_X_AUTO == m_zoomXMode ) {
        // first compute x bounds
        qreal xMin = m_pPlotBScan->getXMin();
        qreal xMax = m_pPlotBScan->getXMax();
        qreal xMiddle = (xMin+xMax / 2);
        xMax = xMiddle + m_AxeXScaleM/2;
        xMin = xMiddle - m_AxeXScaleM/2;

        qreal delta = abs(xMax - xMin);

        qreal margin = delta*0.2;
        if( m_lastPositionM < (xMin+margin) ) {
            xMin = m_lastPositionM - margin;
            xMax = xMin + m_AxeXScaleM;
        }

        if( m_lastPositionM > (xMax-margin) ) {
            xMax = m_lastPositionM + margin;
            xMin = xMax - m_AxeXScaleM;
        }

        m_pPlotBScan->zoom(xMin, xMax,0 ,m_AxeYMaxMm); // X visible area
        needRefresh(true);
    } else if( ZOOM_X_FULL == m_zoomXMode ) {
        m_pPlotBScan->zoom(m_pPlotBScan->getAbsoluteXMin(), m_pPlotBScan->getAbsoluteXMax(),0 ,m_AxeYMaxMm); // X visible area
        needRefresh(true);
    }
}

bool BScanGraph::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_AxeXScaleM, 2);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_AxeYMaxMm, 8);
    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_zoomXMode, ZOOM_X_AUTO);

    return bRet;
}

bool BScanGraph::customLoadDefaultSettings()
{
    bool bRet = true;
    m_AxeXScaleM = 2;
    m_AxeYMaxMm = 8;
    m_zoomXMode = ZOOM_X_AUTO;
    emit evZoomXModeChanged(m_zoomXMode);
    return bRet;
}

bool BScanGraph::customApplySettings()
{
    bool bRet = true;
    updateAxes();
    return bRet;
}

bool BScanGraph::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_AxeXScaleM);
    bRet &= XML_WRITE_UNIQUE(m_AxeYMaxMm);
    bRet &= XML_WRITE_UNIQUE(m_zoomXMode);
    return bRet;
}

void BScanGraph::onPrivateNeedRefresh()
{
    needRefresh(false);
}

void BScanGraph::onThreadFinished()
{
    if( m_timeRefresh->isActive() ) {
        m_timeRefresh->stop();
    }
    delete m_timeRefresh;
    m_timeRefresh = nullptr;
}

void BScanGraph::onUpdateWidgetSize()
{
    // update replot time to get a delay
    m_lastUpdateEpoc = QDateTime::currentMSecsSinceEpoch();
    needRefresh(false);
}

void BScanGraph::onTimerRefresh()
{
    needRefresh(false);
}

void BScanGraph::onPlotMouseMove(double x, double y)
{
    m_mouseLastPos = QPointF(x,y);

    if( m_mouseClicked ) {
        // drag on the graph
        qreal dX = m_mouseClickPressX - m_mouseLastPos.x();
        qreal newXMin = m_pPlotBScan->getXMin()+dX;
        qreal newXMax = m_pPlotBScan->getXMax()+dX;
        if( newXMax > m_pPlotBScan->getAbsoluteXMax() ) {
            newXMax = m_pPlotBScan->getAbsoluteXMax();
            newXMin = newXMax - (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }
        if( newXMin < m_pPlotBScan->getAbsoluteXMin() ) {
            newXMin = m_pPlotBScan->getAbsoluteXMin();
            newXMax = newXMin + (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }

        m_pPlotBScan->zoom(newXMin, newXMax,0 ,m_AxeYMaxMm); // X visible area
        needRefresh(true);
    }        
}

void BScanGraph::onPlotMouseClicked(double x, double y, Qt::KeyboardModifiers, Qt::MouseButton p_Bt)
{
    m_mouseLastPos = QPointF(x,y);

    if( ZOOM_X_AUTO == m_zoomXMode ) {
        setZoomXMode(ZOOM_X_USER) ;
    }

    if(ZOOM_X_USER == m_zoomXMode) {
        if( !m_pPlotBScan->getMouseUsedByItem() && p_Bt == Qt::LeftButton ) {
            // prepare drag : store where mouse has clicked
            m_pPlotBScan->setMouseUsedByItem(true);
            m_mouseClicked = true;
            m_mouseClickPressX = m_mouseLastPos.x();
            m_mouseClickPressAxeXMin = m_pPlotBScan->getAbsoluteXMin();
            m_mouseClickPressAxeXMax = m_pPlotBScan->getAbsoluteXMax();
        }
    }
}
