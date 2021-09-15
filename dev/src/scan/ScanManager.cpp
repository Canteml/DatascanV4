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
#include <scan/AScanGraph.h>
#include <scan/BScanGraph.h>
#include "jkPlot/JkGate.h"
#include "exam/ExamManager.h"

// -- Qt Includes
#include <QQmlApplicationEngine>

ScanManager::ScanManager() : XmlSettingsLoggable("ScanManger")
{
    m_pAScanGraphMain = nullptr;
    m_pGate0 = nullptr;
    m_pGate1 = nullptr;
    m_pVector = nullptr;
    m_selectedAScanIndex = 0;

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
        m_pBScanGraphMain->setParent(this);
        m_pBScanGraphMain->initAfterItemAdded();
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
