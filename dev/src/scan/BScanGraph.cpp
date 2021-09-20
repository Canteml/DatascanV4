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

const qreal BScanGraph::SCALE_X_M_MIN = 2;
const qreal BScanGraph::SCALE_X_M_MAX = 200;
const qreal BScanGraph::SCALE_Y_MM_MAX = 200;

BScanGraph::BScanGraph(QQuickItem *parent) : QQuickPaintedItem(parent), XmlSettingsLoggable("BScanGraph")
{ 
    setAcceptHoverEvents(true);
    setFlag( QQuickItem::ItemHasContents, true );
    setFlag( QQuickItem::ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons( Qt::AllButtons );
    m_updateInProgress = false;
    m_mouseClicked = false;
    m_mouseClickPressX = 0;
    m_axeXScaleM = 2;
    m_axeYMinMm = 0;
    m_axeYMaxMm = 10;
    m_viewXMode = VIEW_X_AUTO;
    m_viewYMode = VIEW_Y_AUTO;


    m_surfaceXMinMm = 0;
    m_surfaceXMaxMm = 0;
    m_surfaceYMinMm = 0;
    m_surfaceYMaxMm = 0;

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
    m_graphBScan->setColor("#80202020"); // opacity @ 50%
    //m_graphBScan->setColor("#202020");
    m_graphBScan->setLineWidth(5);
    m_pPlotBScan->addGraph(m_graphBScan);
    m_pPlotBScan->getPlotter()->setEmittingPlotSignalsEnabled(false);

    m_pPositionLine = new JKQTPGeoLine(m_pPlotBScan, 0, 0, 0, SCALE_Y_MM_MAX);
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
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForText(false);

    // scale
    //m_pPlotAScan->setAbsoluteY(0,100);
    //onDigitilizingDepthChanged();
    //m_pPlotAScan->setAbsoluteX(-1000,1000); // X visible area
    //m_pPlotAScan->getXAxis()->setRange(0,20);
    m_pPlotBScan->setXY(0, 20, 0, 20);
    m_pPlotBScan->getPlotter()->setBackgroundColor(QColor(255,255,255));
    m_pPlotBScan->getPlotter()->setPlotBorder(-10,0,-5,5);

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
        updateAxes();
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

                bool reqUpdateAxes = false;
                if( m_surfaceXMinMm != p_surface->getPositionMin() ) {
                    m_surfaceXMinMm = p_surface->getPositionMin();
                    reqUpdateAxes = true;
                }

                if( m_surfaceXMaxMm != p_surface->getPositionMax() ) {
                    m_surfaceXMaxMm = p_surface->getPositionMax();
                    reqUpdateAxes = true;
                }

                if( m_surfaceYMinMm != p_surface->getMeasureMin() ) {
                    m_surfaceYMinMm = p_surface->getMeasureMin();
                    reqUpdateAxes = true;
                }

                if( m_surfaceYMaxMm != p_surface->getMeasureMax() ) {
                    m_surfaceYMaxMm = p_surface->getMeasureMax();
                    reqUpdateAxes = true;
                }

                //if( reqUpdateAxes ) {
                    updateAxes();
                //} else {
//
 //                   needRefresh(false);
                }
      //      }
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

    customApplySettings();
}

//void BScanGraph::scaleXIn()
//{
//    qreal old = m_AxeXScaleM;
//    if( m_AxeXScaleM <= 4 && m_AxeXScaleM>1 ) {
//        m_AxeXScaleM -= 1;
//    } else if( m_AxeXScaleM <= 10 ) {
//        m_AxeXScaleM -= 2;
//    } else if( m_AxeXScaleM <= 30 ) {
//        m_AxeXScaleM -= 5;
//    } else if( m_AxeXScaleM <= 100 ) {
//        m_AxeXScaleM -= 10;
//    }
//    if( old != m_AxeXScaleM ) {
//        updateAxes();
//    }
//}

//void BScanGraph::scaleXOut()
//{
//    qreal old = m_AxeXScaleM;
//    if( m_AxeXScaleM < 4 && m_AxeXScaleM>1 ) {
//        m_AxeXScaleM += 1;
//    } else if( m_AxeXScaleM < 10 ) {
//        m_AxeXScaleM += 2;
//    } else if( m_AxeXScaleM < 30 ) {
//        m_AxeXScaleM += 5;
//    } else if( m_AxeXScaleM < 100 ) {
//        m_AxeXScaleM += 10;
//    }
//    if( old != m_AxeXScaleM ) {
//        updateAxes();
//    }
//}

//void BScanGraph::thicknessYIn()
//{
//    qreal old = m_axeYMaxMm;
//    if( m_axeYMaxMm <= 10 && m_axeYMaxMm>3 ) {
//        m_axeYMaxMm -= 1;
//    } else if( m_axeYMaxMm <= 20 ) {
//        m_axeYMaxMm -= 2;
//    } else if( m_axeYMaxMm <= 30 ) {
//        m_axeYMaxMm -= 5;
//    } else if( m_axeYMaxMm <= 100 ) {
//        m_axeYMaxMm -= 10;
//    }
//    if( old != m_axeYMaxMm ) {
//        updateAxes();
//    }
//}

//void BScanGraph::thicknessYOut()
//{
//    qreal old = m_axeYMaxMm;
//    if( m_axeYMaxMm < 10 && m_axeYMaxMm>3 ) {
//        m_axeYMaxMm += 1;
//    } else if( m_axeYMaxMm < 20 ) {
//        m_axeYMaxMm += 2;
//    } else if( m_axeYMaxMm < 30 ) {
//        m_axeYMaxMm += 5;
//    } else if( m_axeYMaxMm < 100 ) {
//        m_axeYMaxMm += 10;
//    }
//    if( old != m_axeYMaxMm ) {
//        updateAxes();
//    }
//}

//void BScanGraph::setViewXMode(qint32 p_value)
//{
//    if( m_viewXMode != p_value ) {
//        m_viewXMode = p_value;
//        emit evViewXModeChanged(m_viewXMode);
//    }
//}

//qint32 BScanGraph::getViewXMode() const
//{
//    return m_viewXMode;
//}

// -- Properties

qint32 BScanGraph::getViewXMode(void) const
{
    return m_viewXMode;
}

void BScanGraph::setViewXMode(qint32 p_value)
{
    if( m_viewXMode != p_value ) {
        m_viewXMode = p_value;
        emit evViewXModeChanged(m_viewXMode);
        updateAxes();
    }
}

qint32 BScanGraph::getViewYMode(void) const
{
    return m_viewYMode;
}

void BScanGraph::setViewYMode(qint32 p_value)
{
    if( m_viewYMode != p_value ) {
        m_viewYMode = p_value;
        emit evViewYModeChanged(m_viewYMode);
        updateAxes();
    }
}

qreal BScanGraph::getAxeXScaleMm(void) const
{
    return m_axeXScaleM;
}

void BScanGraph::setAxeXScaleMm(qreal p_value)
{
    if( m_axeXScaleM != p_value ) {
        m_axeXScaleM = p_value;
        emit evAxeXScaleMmChanged(m_axeXScaleM);
        updateAxes();
    }
}

qreal BScanGraph::getAxeYMinMm(void) const
{
    return m_axeYMinMm;
}

void BScanGraph::setAxeYMinMm(qreal p_value)
{
    if( m_axeYMinMm != p_value ) {
        m_axeYMinMm = p_value;
        emit evAxeYMinMmChanged(m_axeYMinMm);
        updateAxes();
    }
}

qreal BScanGraph::getAxeYMaxMm(void) const
{
    return m_axeYMaxMm;
}

void BScanGraph::setAxeYMaxMm(qreal p_value)
{
    if( m_axeYMaxMm != p_value ) {
        m_axeYMaxMm = p_value;
        emit evAxeYMaxMmChanged(m_axeYMinMm);
        updateAxes();
    }
}

// -- Actions

void BScanGraph::axeXScaleIn()
{
    if( SCALE_X_M_MIN < m_axeXScaleM ) {
        setAxeXScaleMm(m_axeXScaleM-1);
    }
}

void BScanGraph::axeXScaleOut()
{
    if( SCALE_X_M_MAX > m_axeXScaleM ) {
        setAxeXScaleMm(m_axeXScaleM+1);
    }
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
        if( dTime >= 50 || p_force ) {
            m_updateInProgress = true;
            update();
        } else if( !m_timeRefresh->isActive() ){
            // start timer with remaining time
            m_timeRefresh->start(50 - (dTime));
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

//    qreal xMin = m_pPlotBScan->getXMin();
//    qreal xMax = m_pPlotBScan->getXMax();
//    qreal d = abs(xMax - xMin);
//    qreal v = d*0.2;
//    if( m_lastPositionM < (xMin+v) || m_lastPositionM < (xMax-v) ) {
//        updateAxes();
//    }

//    // update graph
    m_pPositionLine->setX1(m_lastPositionM);
    m_pPositionLine->setX2(m_lastPositionM);

//    needRefresh(false);
    updateAxes(UP_AXES_POSITION);
}

void BScanGraph::updateAxes(qint32 pSource)
{
    qreal xMin = 0, xMax = 0, yMin = 0, yMax = 0;

    if( VIEW_X_AUTO == m_viewXMode ) {
        if( UP_AXES_POSITION == pSource ) {
            // first compute x bounds
            xMin = m_pPlotBScan->getXMin();
            xMax = m_pPlotBScan->getXMax();
            qreal xMiddle = (xMin+xMax) / 2;
            xMax = xMiddle + m_axeXScaleM/2;
            xMin = xMiddle - m_axeXScaleM/2;

            qreal delta = abs(xMax - xMin);

            qreal margin = delta*0.1;
            if( m_lastPositionM < (xMin+margin) ) {
                xMin = m_lastPositionM - margin;
                xMax = xMin + m_axeXScaleM;
            }

            if( m_lastPositionM > (xMax-margin) ) {
                xMax = m_lastPositionM + margin;
                xMin = xMax - m_axeXScaleM;
            }
        } else {
            xMin = m_pPlotBScan->getXMin();
            xMax = m_pPlotBScan->getXMax();
        }

    } else if( VIEW_X_USER == m_viewXMode ) {
        if( UP_AXES_MOUSE == pSource ) {
            xMin = m_axeXUserMinMm;
            xMax = m_axeXUserMaxMm;
        } else {
            xMin = m_pPlotBScan->getXMin();
            xMax = m_pPlotBScan->getXMax();
        }
    } else if( VIEW_X_FULL == m_viewXMode ) {
        xMin = m_surfaceXMinMm/1000-0.1; // convert into M, add margins
        xMax = m_surfaceXMaxMm/1000+0.1; // convert into M
    }

//    // check limits
//    if( abs(xMax-xMin) < SCALE_X_M_MIN ) {
//        xMin = m_lastPositionM - SCALE_X_M_MIN / 2;
//        xMax = xMin + SCALE_X_M_MIN;
//    } else if( abs(xMax-xMin) > SCALE_X_M_MAX ) {
//        xMin = m_lastPositionM - SCALE_X_M_MAX / 2;
//        xMax = xMin + SCALE_X_M_MAX;
//    }

    if( VIEW_Y_AUTO == m_viewYMode ) {
        // first get y bounds
        yMin = m_surfaceYMinMm;
        yMax = m_surfaceYMaxMm;
        if( yMax == 0 ) {
            yMax = 10;
        }
        if( yMin >= yMax ) {
            yMin = 0;
        }
    } else if( VIEW_Y_USER == m_viewYMode ) {
        yMin = m_axeYMinMm;
        yMax = m_axeYMaxMm;
    } else if( VIEW_Y_FULL == m_viewYMode ) {
        yMin = 0;
        yMax = m_surfaceYMaxMm;
    }

    // check limits
    if( yMin < 0) {
        yMin = 0;
    }
    if( yMax > SCALE_Y_MM_MAX ) {
        yMax = SCALE_Y_MM_MAX;
    }

    //m_pPlotBScan->zoom(xMin, xMax, yMin ,yMax);
    m_pPlotBScan->setXY(xMin, xMax, yMin, yMax);
    needRefresh(false);
}

bool BScanGraph::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_axeXScaleM, 4);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_axeYMinMm, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_axeYMaxMm, 10);

    return bRet;
}

bool BScanGraph::customLoadDefaultSettings()
{
    bool bRet = true;
    m_axeXScaleM = 2;
    m_axeYMinMm = 0;
    m_axeYMaxMm = 10;
    m_viewXMode = VIEW_X_AUTO;
    m_viewYMode = VIEW_Y_AUTO;
    return bRet;
}

bool BScanGraph::customApplySettings()
{
    bool bRet = true;
    updateAxes();
    emit evAxeXScaleMmChanged(m_axeXScaleM);
    emit evAxeYMinMmChanged(m_axeYMinMm);
    emit evAxeYMaxMmChanged(m_axeYMaxMm);
    emit evViewXModeChanged(m_viewXMode);
    emit evViewYModeChanged(m_viewYMode);
    return bRet;
}

bool BScanGraph::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_axeXScaleM);
    bRet &= XML_WRITE_UNIQUE(m_axeYMinMm);
    bRet &= XML_WRITE_UNIQUE(m_axeYMaxMm);
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
        m_axeXUserMinMm = m_pPlotBScan->getXMin()+dX;
        m_axeXUserMaxMm = m_pPlotBScan->getXMax()+dX;
        if( m_axeXUserMaxMm > m_surfaceXMaxMm ) {
            m_axeXUserMaxMm = m_surfaceXMaxMm;
            m_axeXUserMinMm = m_axeXUserMaxMm - (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }
        if( m_axeXUserMinMm < m_surfaceXMinMm ) {
            m_axeXUserMinMm = m_surfaceXMinMm;
            m_axeXUserMaxMm = m_axeXUserMinMm + (m_mouseClickPressAxeXMax - m_mouseClickPressAxeXMin);
        }
        updateAxes(UP_AXES_MOUSE);
    }        
}

void BScanGraph::onPlotMouseClicked(double x, double y, Qt::KeyboardModifiers, Qt::MouseButton p_Bt)
{
    m_mouseLastPos = QPointF(x,y);

    if( VIEW_X_AUTO == m_viewXMode ) {
        setViewXMode(VIEW_X_USER) ;
    }

    if(VIEW_X_USER == m_viewXMode) {
        if( !m_pPlotBScan->getMouseUsedByItem() && p_Bt == Qt::LeftButton ) {
            // prepare drag : store where mouse has clicked
            m_pPlotBScan->setMouseUsedByItem(true);
            m_mouseClicked = true;
            m_mouseClickPressX = m_mouseLastPos.x();
            m_mouseClickPressAxeXMin = m_pPlotBScan->getXMin();
            m_mouseClickPressAxeXMax = m_pPlotBScan->getXMax();
        }
    }
}
