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
#include <v2/scan/Location1d.h>
#include <v2/scan/Zone1d.h>

#include <jkqtplotter/graphs/jkqtpgeometric.h>

#include <QtGlobal>

#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/exam/ExamManager.h>

const qreal BScanGraph::SCALE_X_M_MIN = 1;
const qreal BScanGraph::SCALE_X_M_MAX = 10;
const qreal BScanGraph::SCALE_Y_MM_MAX = 200;

// RENDER MULTI THREAD

BScanGraph::BScanGraph(QQuickItem *parent) : QQuickPaintedItem(parent), XmlSettingsLoggable("BScanGraph")
{
    setAcceptHoverEvents(true);
    setFlag( QQuickItem::ItemHasContents, true );
    setFlag( QQuickItem::ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons( Qt::AllButtons );
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    m_updateInProgress = false;
    m_mouseClicked = false;
    m_mouseClickPressX = 0;
    m_axeXScaleM = 2;
    m_axeYMinMm = 0;
    m_axeYMaxMm = 10;
    m_viewXMode = VIEW_X_AUTO;
    m_viewYMode = VIEW_Y_AUTO;
    m_pLastSurface1d = nullptr;

    resetFlags();

    m_surfaceXMinMm = 0;
    m_surfaceXMaxMm = 0;
    m_surfaceYMinMm = 0;
    m_surfaceYMaxMm = 0;

    m_pExamManager = ExamManager::getInstance();
    connect(m_pExamManager, &ExamManager::evMeasureDisplayChanged, this, &BScanGraph::onMeasureDisplayChanged);

    // Worker

//    m_pBScanWorker = new BScanWorker(this);
//    connect(m_pBScanWorker, &BScanWorker::evUpdateDone, this, &BScanGraph::onBScanReplotted, Qt::QueuedConnection);

    // -- Scan connect

    m_eMouseMove = new QMouseEvent(QEvent::MouseMove, QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);

    connect( this, &QQuickPaintedItem::widthChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &QQuickPaintedItem::heightChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &BScanGraph::widthChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );
    connect( this, &BScanGraph::heightChanged, this, &BScanGraph::onUpdateWidgetSize, Qt::QueuedConnection );

    connect( VectorManager::getInstance()->getVectorStd()->funcPosition(), &VectorFuncPosition::paramLBscanPosMmXChanged, this, &BScanGraph::onPositionChanged);

    // -- Make BSCAN
    m_pPlotBScan = new MyJkQtPlotter();
    m_dsBScan = m_pPlotBScan->getDatastore();

    // -- Graphic items

    // make fake graph to init columns
    QVector<qreal> vX, vY;
    vX.append(0);
    vY.append(0);
    m_columnX=m_dsBScan->addColumn(vX.data(), 1, "x");
    m_columnY=m_dsBScan->addColumn(vY.data(), 1, "y");

    m_graphBScan=new JKQTPImpulsesVerticalGraph(m_pPlotBScan);
    m_graphBScan->setXColumn(m_columnX);
    m_graphBScan->setYColumn(m_columnY);
    m_graphBScan->setSymbolType(JKQTPNoSymbol);
    m_graphBScan->setColor("#80202020"); // opacity @ 50% => Hight CPU usage
    //m_graphBScan->setColor("#202020");
    m_graphBScan->setLineWidth(1);
    m_pPlotBScan->addGraph(m_graphBScan);
    m_pPlotBScan->getPlotter()->setEmittingPlotSignalsEnabled(false);   ///< we manage redraw

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
    m_pPlotBScan->getYAxis()->setDrawMode2(JKQTPCADrawMode::JKQTPCADMLineTicksTickLabels );
    m_pPlotBScan->getYAxis()->setInverted(true);

    // improve performances
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForGraphs(true);
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForSystem(true);
    m_pPlotBScan->getPlotter()->setUseAntiAliasingForText(false);

    // scale   
    m_drawFlagViewXMode = true; // force update
    m_pPlotBScan->setXY(0, 20, -m_axeXScaleM/2, m_axeXScaleM/2);
    m_pPlotBScan->getPlotter()->setBackgroundColor(QColor(255,255,255));
    m_pPlotBScan->getPlotter()->setPlotBorder(-10,0,-5,5);

    connect(m_pPlotBScan, &MyJkQtPlotter::evRequestUpdate, this, &BScanGraph::onPrivateNeedRefresh, Qt::QueuedConnection);
    connect(this, &BScanGraph::evPrivateNeedRefresh, this, &BScanGraph::onPrivateNeedRefresh);

    // -- Timer
    m_lastRequestEpoc = QDateTime::currentMSecsSinceEpoch();
    m_timeRefresh = new QTimer();
    m_timeRefresh->setSingleShot(true);
    connect(m_timeRefresh, &QTimer::timeout, this, &BScanGraph::onTimerRefresh, Qt::QueuedConnection);




}

BScanGraph::~BScanGraph()
{
//    delete m_pBScanWorker;
//    m_pBScanWorker = nullptr;
    m_pPlotBScan->deleteLater();
    m_pPlotBScan = nullptr;
    delete m_eMouseMove;
    m_eMouseMove = nullptr;
}

void BScanGraph::stop()
{
//    m_pBScanWorker->stop();
    disconnect(m_pExamManager, &ExamManager::evRequestSurfaceUpdate, this, &BScanGraph::onUpdateBScan);
    disconnect(m_pExamManager, &ExamManager::evRequestLocationZoneUpdate, this, &BScanGraph::onUpdateLocationZone);
    m_pLastSurface1d = nullptr;
    QVector<qreal> v;
    v.append(0);
    m_dsBScan->setColumnData(m_columnX, v);
    m_dsBScan->setColumnData(m_columnY, v);
}

void BScanGraph::paint(QPainter *painter)
{
    m_pPlotBScan->render(painter);
    //painter->drawImage(QRect(0,0, width(), height()),m_buffer);
    m_updateInProgress = false;
//    if( nullptr != m_pBScanWorker ) {
//        m_pBScanWorker->signalPaintDone();
//    }
}

void BScanGraph::onUpdateBScan(Surface1d* p_surface)
{
    m_pLastSurface1d = p_surface;
    processLastSurface();
    needRefresh(false);
}

void BScanGraph::processLastSurface()
{
    if( !m_mouseClicked ) {
        if( nullptr != m_pLastSurface1d ) {
            qint32 nbCell =  m_pLastSurface1d->getNbCell();
            if( 0 != nbCell ) {
                m_pLastSurface1d->lockData();
                // less efficient than addColumn, but should be \_('_')_/
//                m_dsBScan->setColumnData(m_columnX, *m_pLastSurface1d->getArrayPosition());
//                m_dsBScan->setColumnData(m_columnY, *m_pLastSurface1d->getArrayMeasure());
                m_columnX=m_dsBScan->addColumn(m_pLastSurface1d->getArrayPosition()->data(), nbCell, "x");

                if( m_pExamManager->getMeasureDisplay() == ExamManager::MEASURE_1E ) {
                    m_columnY=m_dsBScan->addColumn(m_pLastSurface1d->getArrayMeasure1e()->data(), nbCell, "y");
                } else {
                    m_columnY=m_dsBScan->addColumn(m_pLastSurface1d->getArrayMeasureMe()->data(), nbCell, "y");
                }

                m_pLastSurface1d->unlockData();

                m_graphBScan->setXColumn(m_columnX);
                m_graphBScan->setYColumn(m_columnY);

                if( m_surfaceXMinMm != m_pLastSurface1d->getPositionMin() ) {
                    m_surfaceXMinMm = m_pLastSurface1d->getPositionMin();
                    m_drawFlagSurfaceRange = true;
                }

                if( m_surfaceXMaxMm != m_pLastSurface1d->getPositionMax() ) {
                    m_surfaceXMaxMm = m_pLastSurface1d->getPositionMax();
                    m_drawFlagSurfaceRange = true;
                }

                if( m_surfaceYMinMm != m_pLastSurface1d->getMeasureMin() ) {
                    m_surfaceYMinMm = m_pLastSurface1d->getMeasureMin();
                    m_drawFlagSurfaceRange = true;
                }

                if( m_surfaceYMaxMm != m_pLastSurface1d->getMeasureMax() ) {
                    m_surfaceYMaxMm = m_pLastSurface1d->getMeasureMax();
                    m_drawFlagSurfaceRange = true;
                }
            }
        } else {
            // clear scan
            QVector<qreal> v;
            v.append(0);
            m_dsBScan->setColumnData(m_columnX, v);
            m_dsBScan->setColumnData(m_columnY, v);
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
    connect(m_pExamManager, &ExamManager::evRequestSurfaceUpdate, this, &BScanGraph::onUpdateBScan);
    connect(m_pExamManager, &ExamManager::evRequestLocationZoneUpdate, this, &BScanGraph::onUpdateLocationZone);

    customApplySettings();

    needRefresh(true);

    //m_pBScanWorker->start();
}

// -- Properties

qint32 BScanGraph::getViewXMode(void) const
{
    return m_viewXMode;
}

void BScanGraph::setViewXMode(qint32 p_value)
{
    if( m_viewXMode != p_value ) {
        m_viewXMode = p_value;

        // manage BScan render function to view to optimize CPU
//        if( VIEW_X_FULL == m_viewXMode ) {
//            // reduce CPU usage
//            m_graphBScan->setColor("#202020"); // opacity @ 50% => Hight CPU usage
//            m_pPlotBScan->getPlotter()->setUseAntiAliasingForGraphs(false);
//            m_pPlotBScan->getPlotter()->setUseAntiAliasingForSystem(false);
//        } else {
//            m_graphBScan->setColor("#80202020");
//            m_pPlotBScan->getPlotter()->setUseAntiAliasingForGraphs(true);
//            m_pPlotBScan->getPlotter()->setUseAntiAliasingForSystem(true);
//        }
        m_drawFlagViewXMode = true;
        needRefresh();
        emit evViewXModeChanged(m_viewXMode);
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
        m_drawFlagViewYMode = true;
        needRefresh();
        emit evViewYModeChanged(m_viewYMode);
    }
}

qreal BScanGraph::getAxeXScaleMm(void) const
{
    return m_axeXScaleM;
}

void BScanGraph::setAxeXScaleMm(qreal p_value)
{
    if( p_value > SCALE_X_M_MAX ) {
        p_value = SCALE_X_M_MAX;
    }
    if( p_value < SCALE_X_M_MIN ) {
        p_value = SCALE_X_M_MIN;
    }
    if( m_axeXScaleM != p_value ) {
        m_axeXScaleM = p_value;
        m_drawFlagScaleX = true;
        needRefresh();
        emit evAxeXScaleMmChanged(m_axeXScaleM);
    }
}

qreal BScanGraph::getAxeYMinMm(void) const
{
    return m_axeYMinMm;
}

void BScanGraph::setAxeYMinMm(qreal p_value)
{
    if( p_value < 0 ) {
        p_value = 0;
    }
    if( p_value > m_axeYMaxMm ) {
        p_value = m_axeYMinMm;
    }

    if( m_axeYMinMm != p_value ) {
        m_axeYMinMm = p_value;
        emit evAxeYMinMmChanged(m_axeYMinMm);
        m_drawFlagScaleY = true;
        needRefresh();
    }
}

qreal BScanGraph::getAxeYMaxMm(void) const
{
    return m_axeYMaxMm;
}

void BScanGraph::setAxeYMaxMm(qreal p_value)
{
    if( p_value < m_axeYMinMm ) {
        p_value = m_axeYMinMm;
    }
    if( p_value > SCALE_Y_MM_MAX ) {
        p_value = SCALE_Y_MM_MAX;
    }
    if( m_axeYMaxMm != p_value ) {
        m_axeYMaxMm = p_value;
        emit evAxeYMaxMmChanged(m_axeYMinMm);        
        m_drawFlagScaleY = true;
        needRefresh();
    }
}

// -- Actions

//void BScanGraph::axeXScaleIn()
//{
//    if( SCALE_X_M_MIN < m_axeXScaleM ) {
//        setAxeXScaleMm(m_axeXScaleM-1);
//        m_drawFlagScaleX = true;
//        needRefresh();
//    }
//}

//void BScanGraph::axeXScaleOut()
//{
//    if( SCALE_X_M_MAX > m_axeXScaleM ) {
//        setAxeXScaleMm(m_axeXScaleM+1);
//        m_drawFlagScaleX = true;
//        needRefresh();
//    }
//}

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
    if( !m_timeRefresh->isActive() || p_force) {
        qint64 dTime = curEpoc - m_lastRequestEpoc;
        if( dTime >= 50 || p_force ) {
            m_lastRequestEpoc = curEpoc;
            m_updateInProgress = true;
            draw();
            update();
        } else if( !m_timeRefresh->isActive() ){
            // start timer with remaining time
            m_timeRefresh->start(50 - (dTime));
        }
    }

//    if(  nullptr != m_pBScanWorker ) {
//        m_pBScanWorker->requestUpdate();
//    }
}

void BScanGraph::draw()
{
    // need resize ?
    if( nullptr != m_pPlotBScan ) {
        if( m_pPlotBScan->width() != width() || m_pPlotBScan->height() != height() ) {
            m_pPlotBScan->resize(width(), height());
            emit evPrivateNeedRefresh();
        } else {

            //processLastSurface(); // multithread
            updateAxes();

            // update line width function to window size, x scale and resolution :
            if( nullptr != m_pLastSurface1d ) {
                qreal wp = m_pPlotBScan->getPlotter()->getPlotWidth();

                // on cherche une largeur donnant des barres jointives sans recouvrement pour avoir
                // un rendu uniforme sans utiliser l'anti-aliasing :
                qreal r = wp/((m_axeXScaleM*1000)/m_pLastSurface1d->getResolution());
                // take care of DPI because line width is set in Point (not in pixel as we want)
                qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
                r= r*72/dpi;
                if( r != m_graphBScan->getLineWidth() ) {
                    m_graphBScan->setLineWidth(r);
                }
            }

            if( m_pExamManager->getMeasureDisplay() == ExamManager::MEASURE_1E ) {
                m_graphBScan->setColor("#80801010");
            } else {
                m_graphBScan->setColor("#80202020"); // opacity @ 50% => Hight CPU usage
            }

            if( m_drawFlagPosition ) {
                m_pPositionLine->setX1(m_lastPositionM);
                m_pPositionLine->setX2(m_lastPositionM);
            }
            resetFlags();

            m_pPlotBScan->redrawPlot();

//            if( m_buffer.width() != width() || m_buffer.height() !=  height() ) {
//                m_buffer = QImage(width(), height(), QImage::Format_ARGB32);
//            }
//            qreal magnification = m_pPlotBScan->getMagnification();
//            m_pPlotBScan->getPlotter()->setWidgetSize(jkqtp_roundTo<int>(width()/magnification), jkqtp_roundTo<int>(height()/magnification));
//            JKQTPEnhancedPainter t(&m_buffer);
//            t.scale(magnification, magnification);
//            m_pPlotBScan->getPlotter()->draw(t, QRect(0,0,width(), height()));
        }
    }
}

void BScanGraph::resetFlags()
{
    m_drawFlagPosition = false;
    m_drawFlagViewXMode = false;
    m_drawFlagViewYMode = false;
    m_drawFlagSurfaceRange = false;
    m_drawFlagUserAxeX = false;
    m_drawFlagUserAxeY = false;
    m_drawFlagScaleX = false;
    m_drawFlagScaleY = false;
}

void BScanGraph::deleteZones()
{
    for(auto i : m_lZoneLine) {
        m_pPlotBScan->deleteGraph(i);
    }
    m_lZoneLine.clear();
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
    m_drawFlagPosition = true;
    needRefresh(false);
}

void BScanGraph::onUpdateLocationZone(Location1d *p_location)
{
    deleteZones();

    const QList<Zone1d*> * pLZ = p_location->getZones();
    if( !pLZ->isEmpty() ) {
        for(auto i : *pLZ) {
            qreal pos = i->getOriginXMm()/1000;
            JKQTPGeoLine * pLine = new JKQTPGeoLine(m_pPlotBScan, pos, 0, pos, SCALE_Y_MM_MAX);
            pLine->setStyle(QColor("#DD00DD"), 2); // Orange : FF8200
            m_pPlotBScan->addGraph(pLine);
            m_lZoneLine.append(pLine);
        }

        if( pLZ->last()->isSizeDefined() ) {
            qreal pos = pLZ->last()->getOriginXMm() + pLZ->last()->getSizeXMm()/1000;
            JKQTPGeoLine * pLine = new JKQTPGeoLine(m_pPlotBScan, pos, 0, pos, SCALE_Y_MM_MAX);
            pLine->setStyle(QColor("#DD00DD"), 2);
            m_pPlotBScan->addGraph(pLine);
            m_lZoneLine.append(pLine);
        }
    }
    needRefresh(false);
}

void BScanGraph::onBScanReplotted()
{
    update();
}

void BScanGraph::onMeasureDisplayChanged(quint32)
{
     processLastSurface();
}

void BScanGraph::updateAxes()
{
    qreal xMin = m_pPlotBScan->getXMin(), xMax = m_pPlotBScan->getXMax(), yMin = m_pPlotBScan->getYMin(), yMax = m_pPlotBScan->getYMax();
    bool updateView = false;

    if( VIEW_X_AUTO == m_viewXMode || m_drawFlagViewXMode ) {
        if( m_drawFlagPosition || m_drawFlagScaleX  ) {
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
            updateView = true;
        } else {
            if( m_drawFlagViewXMode || m_drawFlagViewXMode || m_drawFlagScaleX ) {
                xMin = m_lastPositionM - m_axeXScaleM/2;
                xMax = m_lastPositionM + m_axeXScaleM/2;
                updateView = true;
            }
        }

    } else if( VIEW_X_USER == m_viewXMode ) {
        if( m_drawFlagUserAxeX || m_drawFlagScaleX ) {
            if( m_drawFlagScaleX ) {
                m_axeXUserMinM = m_pPlotBScan->getXMin();// init
            }
            xMin = m_axeXUserMinM;
            xMax = xMin+m_axeXScaleM;
            updateView = true;
        }
    } else if( VIEW_X_FULL == m_viewXMode ) {
        if( m_drawFlagSurfaceRange || m_drawFlagViewXMode || m_drawFlagScaleX ) {
            xMin = qMin(m_lastPositionM, m_surfaceXMinMm/1000); // convert into M
            xMax = qMax(m_lastPositionM, m_surfaceXMaxMm/1000); // convert into M

            // too small => update
            qreal delta = xMax - xMin;
            if( delta<m_axeXScaleM ) {
                xMin = xMin + delta/2 - m_axeXScaleM/2;
                xMax = xMin + m_axeXScaleM;
            }
            qreal margin = (xMax - xMin)/100;
            xMin -= margin;
            xMax += margin;
            updateView = true;
        }
    }

    if( VIEW_Y_AUTO == m_viewYMode ) {
        if( m_drawFlagSurfaceRange || m_drawFlagViewYMode ) {
            // first get y bounds
            yMin = m_surfaceYMinMm;
            yMax = m_surfaceYMaxMm;
            if( yMax == 0 ) {
                yMax = 10;
            }
            if( yMin >= yMax ) {
                yMin = 0;
            }
            updateView = true;
        }
    } else if( VIEW_Y_USER == m_viewYMode ) {
        if( m_drawFlagUserAxeY || m_drawFlagViewYMode || m_drawFlagScaleY) {
            yMin = m_axeYMinMm;
            yMax = m_axeYMaxMm;
            updateView = true;
        }
    } else if( VIEW_Y_FULL == m_viewYMode ) {
        if( m_drawFlagSurfaceRange || m_drawFlagViewYMode ) {
            yMin = 0;
            yMax = m_surfaceYMaxMm;
            updateView = true;
        }
    }

    // check limits
    if( yMin < 0) {
        yMin = 0;
    }
    if( yMax > SCALE_Y_MM_MAX ) {
        yMax = SCALE_Y_MM_MAX;
    }

    if( updateView ) {
        m_pPlotBScan->setXY(xMin, xMax, yMin, yMax);
    }
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
    //m_lastRequestEpoc = QDateTime::currentMSecsSinceEpoch();
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
        m_axeXUserMinM = m_pPlotBScan->getXMin()+dX;
        m_drawFlagUserAxeX = true;
        needRefresh(false);
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

// ----------------------------

//BScanWorker::BScanWorker(BScanGraph * p_parent) : Loggable("BScanWorker")
//{
//    m_pParent = p_parent;
//    m_requestUpdate = false;
//    m_processing = false;

//    m_pThread = new QThread();
//    this->moveToThread(m_pThread);

//    connect(this, &BScanWorker::evRequestUpdate, this, &BScanWorker::onProcessUpdate, Qt::QueuedConnection);
//}

//BScanWorker::~BScanWorker()
//{
//    if(m_pThread->isRunning()) {
//        m_pThread->quit();
//        m_pThread->wait(500);
//    }
//    delete m_pThread;
//    m_pThread = nullptr;
//}

//void BScanWorker::requestUpdate()
//{
//    if( m_processing ) {
//        m_requestUpdate = true;
//    } else {
//        m_processing = true;
//        m_requestUpdate = false;
//        emit evRequestUpdate();
//    }
//}

//void BScanWorker::signalPaintDone()
//{
//    if( m_requestUpdate ) {
//        emit evRequestUpdate();
//    } else {
//        m_processing = false;
//        //fLogDebug2("processing off");
//    }
//}

//void BScanWorker::start()
//{
//    m_pThread->start();
//}

//void BScanWorker::stop()
//{
//    disconnect(this, &BScanWorker::evRequestUpdate, this, &BScanWorker::onProcessUpdate);
//    m_pThread->quit();
//    m_pThread->wait(500);
//}

//void BScanWorker::onProcessUpdate()
//{
//    m_pParent->draw();
//    emit evUpdateDone();
//}
