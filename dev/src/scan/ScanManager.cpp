/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AscanModel.h
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

// -- Internal includes
#include "ScanManager.h"
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/scan/Location1d.h>
#include <scan/AScanGraph.h>
#include <scan/BScanGraph.h>
#include "jkPlot/JkGate.h"
#include <v2/exam/ExamManager.h>

// -- Qt Includes
#include <QQmlApplicationEngine>

ScanManager::ScanManager() : XmlSettingsLoggable("ScanManger")
{
    m_pAScanGraphMain = nullptr;
    m_pBScanGraphMain = nullptr;
    m_pGate0 = nullptr;
    m_pGate1 = nullptr;
    m_pVector = nullptr;
    m_selectedAScanIndex = 0;

    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        m_pAScan[i].init();
    }

}

ScanManager::~ScanManager()
{
    if( nullptr != m_pGate0) {
        delete m_pGate0;
        m_pGate0 = nullptr;
    }
    if( nullptr != m_pGate1) {
        delete m_pGate1;
        m_pGate1 = nullptr;
    }

//    if( nullptr != m_pAScanGraphMain ) {
//        delete m_pAScanGraphMain;
//        m_pAScanGraphMain = nullptr;
//    }
//    if( nullptr != m_pBScanGraphMain ) {
//        delete m_pAScanGraphMain;   //<= crash at close... remove the parent link instead. Dirty solution
//        //m_pBScanGraphMain->setParent(nullptr);
//        m_pBScanGraphMain = nullptr;
//    }
}

void ScanManager::init()
{
    m_pVector = VectorManager::getInstance()->getVectorStd();
    m_pExamManager = ExamManager::getInstance();

    connect(m_pVector->funcAScan0(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan1(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan2(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan3(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector, &VectorStd::evNbFuncAScanEnabledChanged, this, &ScanManager::onVectorNbAScanEnabledChanged);

    connect(m_pExamManager, &ExamManager::evExamStepChanged, this, &ScanManager::onExamManagerStepChanged);
}

void ScanManager::initQmlObject(QObject * p_pQmlTopLevel)
{
    m_pAScanGraphMain = p_pQmlTopLevel->findChild<AScanGraph*>("AScanGraphMain");    
    if( nullptr == m_pAScanGraphMain ) {
        fLogError("Fail to found m_pAScanGraphMain");
    } else {
        m_pAScanGraphMain->setParent(this);
        m_pGate0 = new JkGate(m_pAScanGraphMain->getPlot(), 0);
        m_pGate1 = new JkGate(m_pAScanGraphMain->getPlot(), 1);
        setPrivateSelectedAScanIndex(0);
        m_pAScanGraphMain->initAfterItemAdded();
    }

    m_pBScanGraphMain = p_pQmlTopLevel->findChild<BScanGraph*>("BScanGraphMain");
    if( nullptr == m_pBScanGraphMain ) {
        fLogError("Fail to found m_pBScanGraphMain");
    } else {
        // Proxy connections
        connect(m_pBScanGraphMain, &BScanGraph::evViewXModeChanged, this, &ScanManager::onPrivateBScanViewXModeChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evViewYModeChanged, this, &ScanManager::onPrivateBScanViewYModeChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeXScaleMmChanged, this, &ScanManager::onPrivateBScanAxeXScaleMChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeYMinMmChanged, this, &ScanManager::onPrivateBScanAxeYMinMmChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeYMaxMmChanged, this, &ScanManager::onPrivateBScanAxeYMaxMmChanged);

        m_pBScanGraphMain->setParent(this);
        m_pBScanGraphMain->initAfterItemAdded();
    }
}

void ScanManager::stop()
{    
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->stop();
    }
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->stop();
        disconnect(m_pBScanGraphMain, &BScanGraph::evViewXModeChanged, this, &ScanManager::onPrivateBScanViewXModeChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evViewYModeChanged, this, &ScanManager::onPrivateBScanViewYModeChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeXScaleMmChanged, this, &ScanManager::onPrivateBScanAxeXScaleMChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeYMinMmChanged, this, &ScanManager::onPrivateBScanAxeYMinMmChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeYMaxMmChanged, this, &ScanManager::onPrivateBScanAxeYMaxMmChanged);
    }
}

quint32 ScanManager::getSelectedAScanIndex() const
{
    return m_selectedAScanIndex;
}

void ScanManager::setSelectedAScanIndex(quint32 p_index)
{
    if( p_index != m_selectedAScanIndex ) {
        setPrivateSelectedAScanIndex(p_index);
    }
}

VectorFuncAScan* ScanManager::getSelectedFuncAScan() const
{
    return m_pVector->getFuncAScan(m_selectedAScanIndex);
}

AScan *ScanManager::getCurrentAScan()
{
    return &(m_pAScan[m_selectedAScanIndex]);
}

void ScanManager::ascanZoomIn()
{
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->zoomIn();
    }
}

void ScanManager::ascanZoomOut()
{
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->zoomOut();
    }
}

qreal ScanManager::getAScanThicknessMmMultiEcho()
{
    return getCurrentAScan()->getAScanMeasure()->m_thicknessMmMultiEcho;
}

qreal ScanManager::getAScanThicknessMm1Echo()
{
    return getCurrentAScan()->getAScanMeasure()->m_thicknessMm1Echo;
}

// -- BSCAN

qint32 ScanManager::getBScanViewXMode(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getViewXMode();
    }
    return BScanGraph::VIEW_X_AUTO;
}

void ScanManager::setBScanViewXMode(quint32 p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->setViewXMode(p_value);
    }
}

qint32 ScanManager::getBScanViewYMode(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getViewYMode();
    }
    return BScanGraph::VIEW_Y_AUTO;
}

void ScanManager::setBScanViewYMode(quint32 p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->setViewYMode(p_value);
    }
}

qreal ScanManager::getBScanAxeXScaleM(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeXScaleMm();
    }
    return BScanGraph::SCALE_X_M_MIN;
}

void ScanManager::setBScanAxeXScaleM(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeXScaleMm(p_value);
    }
}

qreal ScanManager::getBScanAxeYMinMm(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeYMinMm();
    }
    return 0;
}

void ScanManager::setBScanAxeYMinMm(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeYMinMm(p_value);
    }
}

qreal ScanManager::getBScanAxeYMaxMm(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeYMaxMm();
    }
    return 0;
}

void ScanManager::setBScanAxeYMaxMm(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeYMaxMm(p_value);
    }
}

void ScanManager::onNewAScanTreated(AScan * p_pAscan)
{
    if( p_pAscan->m_source == m_selectedAScanIndex && nullptr != m_pAScanGraphMain ) {
        m_pAScan[p_pAscan->m_source].copy(*p_pAscan);
        m_pAScanGraphMain->updateAScan(&m_pAScan[p_pAscan->m_source]);
        emit evUpdateAScan();
    }
    m_pExamManager->pushAScan(p_pAscan);
}

void ScanManager::onVectorNbAScanEnabledChanged(qint32)
{
//    // clear ASCAN
//    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
//        delete m_pAScan[i];
//        m_pAScan[i] = nullptr;
//    }
    setSelectedAScanIndex(0);
}

void ScanManager::onPrivateBScanViewXModeChanged(qint32 p_value)
{
    emit evBScanViewXModeChanged(p_value);
}

void ScanManager::onPrivateBScanViewYModeChanged(qint32 p_value)
{
    emit evBScanViewYModeChanged(p_value);
}

void ScanManager::onPrivateBScanAxeXScaleMChanged(qreal p_value)
{
    emit evBScanAxeXScaleMChanged(p_value);
}

void ScanManager::onPrivateBScanAxeYMinMmChanged(qreal p_value)
{
    emit evBScanAxeYMinMmChanged(p_value);
}

void ScanManager::onPrivateBScanAxeYMaxMmChanged(qreal p_value)
{
    emit evBScanAxeYMaxMmChanged(p_value);
}

void ScanManager::onExamManagerStepChanged(qint32 p_value)
{
    if( p_value == ExamManager::EXAM_EXPORTING ) {
        // Go to view FULL
        if( m_pBScanGraphMain->getViewXMode() != BScanGraph::VIEW_X_FULL ) {
            m_pBScanGraphMain->setViewXMode(BScanGraph::VIEW_X_FULL);
        }
        if(  m_pBScanGraphMain->getViewYMode() != BScanGraph::VIEW_Y_FULL ) {
            m_pBScanGraphMain->setViewYMode(BScanGraph::VIEW_Y_FULL);
        }
    } else if( p_value == ExamManager::EXAM_RECORDING || p_value == ExamManager::EXAM_STOPPED  || p_value == ExamManager::EXAM_PAUSED) {
        // Go to Auto if full
        if( m_pBScanGraphMain->getViewXMode() != BScanGraph::VIEW_X_AUTO ) {
             m_pBScanGraphMain->setViewXMode(BScanGraph::VIEW_X_AUTO);
        }
        if( m_pBScanGraphMain->getViewYMode() != BScanGraph::VIEW_Y_AUTO ) {
            m_pBScanGraphMain->setViewYMode(BScanGraph::VIEW_Y_AUTO);
        }
    }
}

bool ScanManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    if( nullptr != m_pAScanGraphMain ) {
        bRet &= m_pAScanGraphMain->loadSettings(p_pParent);
    }
    if( nullptr != m_pBScanGraphMain ) {
        bRet &= m_pBScanGraphMain->loadSettings(p_pParent);
    }
    return bRet;
}

bool ScanManager::customLoadDefaultSettings()
{
    bool bRet = true;

    return bRet;
}

bool ScanManager::customApplySettings()
{
    bool bRet = true;

    return bRet;
}

bool ScanManager::customSaveSettings(QDomDocument * /*p_pDoc*/, QDomElement * p_pParent)
{
    bool bRet = true;
    if( nullptr != m_pAScanGraphMain ) {
        bRet &= m_pAScanGraphMain->saveSettings(p_pParent);
    }
    if( nullptr != m_pBScanGraphMain ) {
        bRet &= m_pBScanGraphMain->saveSettings(p_pParent);
    }
    return bRet;
}


void ScanManager::setPrivateSelectedAScanIndex(quint32 p_index)
{
    m_selectedAScanIndex = p_index;
    emit evSelectedAScanIndexChanged(m_selectedAScanIndex);
    emit evSelectedFuncAScanChanged(getSelectedFuncAScan());
    m_pAScanGraphMain->updateAScan(&m_pAScan[p_index]);
    m_pGate0->setAScan(getSelectedFuncAScan());
    m_pGate1->setAScan(getSelectedFuncAScan());
    emit evUpdateAScan();
}
