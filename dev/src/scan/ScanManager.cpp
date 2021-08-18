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
#include "jkPlot/JkGate.h"

// -- Qt Includes
#include <QQmlApplicationEngine>

ScanManager::ScanManager() : XmlSettingsLoggable("ScanManger")
{
    m_pAScanGraphMain = nullptr;
    m_pGate0 = nullptr;
    m_pGate1 = nullptr;
    m_pVector = nullptr;
    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        m_pAScan[i] = nullptr;
    }
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

    connect(m_pVector->funcAScan0(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScan0Treated);
    connect(m_pVector->funcAScan1(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScan1Treated);
    connect(m_pVector->funcAScan2(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScan2Treated);
    connect(m_pVector->funcAScan3(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScan3Treated);
    connect(m_pVector, &VectorStd::evNbFuncAScanEnabledChanged, this, &ScanManager::onVectorNbAScanEnabledChanged);
}

void ScanManager::initQmlObject(QObject * p_pQmlTopLevel)
{
    m_pAScanGraphMain = p_pQmlTopLevel->findChild<AScanGraph*>("AScanGraphMain");
    m_pAScanGraphMain->setParent(this);
    if( nullptr == m_pAScanGraphMain ) {
        fLogError("Fail to found m_pAScanGraphMain");
    } else {
        m_pGate0 = new JkGate(m_pAScanGraphMain->getPlot(), 0);
        m_pGate1 = new JkGate(m_pAScanGraphMain->getPlot(), 1);
        setPrivateSelectedAScanIndex(0);
        m_pAScanGraphMain->initAfterItemAdded();
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

void ScanManager::onNewAScan0Treated(AScan * p_pAscan)
{
    if( nullptr != m_pAScan[0] ) {
        delete m_pAScan[0];
        m_pAScan[0] = nullptr;
    }
    m_pAScan[0] = p_pAscan;
    if( 0 == m_selectedAScanIndex ) {
        m_pAScanGraphMain->updateAScan(p_pAscan);
    }
}

void ScanManager::onNewAScan1Treated(AScan * p_pAscan)
{
    if( nullptr != m_pAScan[1] ) {
        delete m_pAScan[1];
        m_pAScan[1] = nullptr;
    }
    m_pAScan[1] = p_pAscan;
    if( 1 == m_selectedAScanIndex ) {
        m_pAScanGraphMain->updateAScan(p_pAscan);
    }
}

void ScanManager::onNewAScan2Treated(AScan * p_pAscan)
{
    if( nullptr != m_pAScan[2] ) {
        delete m_pAScan[2];
        m_pAScan[2] = nullptr;
    }
    m_pAScan[2] = p_pAscan;
    if( 2 == m_selectedAScanIndex ) {
        m_pAScanGraphMain->updateAScan(p_pAscan);
    }
}

void ScanManager::onNewAScan3Treated(AScan * p_pAscan)
{
    if( nullptr != m_pAScan[3] ) {
        delete m_pAScan[3];
        m_pAScan[3] = nullptr;
    }
    m_pAScan[3] = p_pAscan;
    if( 3 == m_selectedAScanIndex ) {        
        m_pAScanGraphMain->updateAScan(p_pAscan);
    }
}

void ScanManager::onVectorNbAScanEnabledChanged(qint32)
{
    // clear ASCAN
    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        delete m_pAScan[i];
        m_pAScan[i] = nullptr;
    }
    setSelectedAScanIndex(0);
}


bool ScanManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_pAScanGraphMain->loadSettings(p_pParent);
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
    bRet &= m_pAScanGraphMain->saveSettings(p_pParent);
    return bRet;
}


void ScanManager::setPrivateSelectedAScanIndex(quint32 p_index)
{
    m_selectedAScanIndex = p_index;
    emit evSelectedAScanIndexChanged(m_selectedAScanIndex);
    emit evSelectedFuncAScanChanged(getSelectedFuncAScan());
    m_pAScanGraphMain->updateAScan(m_pAScan[p_index]);
    m_pGate0->setAScan(getSelectedFuncAScan());
    m_pGate1->setAScan(getSelectedFuncAScan());
}
