/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : GenComLink.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>


VectorManager * VectorManager::m_pMyInstance = nullptr;


VectorManager::VectorManager(QObject *parent) : QObject(parent), XmlSettingsLoggable("VectorManager")
{
    m_pVectorStd = nullptr;

    makeVectorStd();
}

VectorManager::~VectorManager()
{
    quit();
    m_pMyInstance = nullptr;
}

VectorManager *VectorManager::makeInstance(QObject *parent)
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("VectorManager::makeInstance : instance not null.");
    } else {
        m_pMyInstance = new VectorManager(parent);
    }
    return m_pMyInstance;
}

VectorManager *VectorManager::getInstance()
{
    return m_pMyInstance;
}

// -- Manager
bool VectorManager::init()
{
    bool bRet = true;

    return bRet;
}

bool VectorManager::quit()
{
    bool bRet = true;
    if( nullptr != m_pVectorStd ) {
        m_pVectorStd->destroyVector();
        delete m_pVectorStd;
        m_pVectorStd = nullptr;
    }
    return bRet;
}

void VectorManager::makeVectorStd()
{
    if( nullptr != m_pVectorStd ) {
        fLogError("VectorStd already made.");
    } else {
        m_pVectorStd = new VectorStd(this);
    }
}


VectorStd * VectorManager::getVectorStd()
{
    return m_pVectorStd;
}

// -- Inherited behaviour
bool VectorManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    if( nullptr != m_pVectorStd ) {
        QDomElement nodeVector = XmlHelper::xmlGetChild(p_pParent, "Vector");
        if( !nodeVector.isNull() ) {
            m_pVectorStd->loadParam(&nodeVector);
        }
    }
    return bRet;
}

bool VectorManager::customLoadDefaultSettings()
{
    bool bRet = true;

    return bRet;
}

bool VectorManager::customApplySettings()
{
    bool bRet = true;

    return bRet;
}

bool VectorManager::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    Q_UNUSED(p_pDoc);
    bool bRet = true;
    if( nullptr != m_pVectorStd ) {
        QDomElement nodeVector = XmlHelper::xmlAddUniqueChild(p_pDoc,  p_pParent, "Vector");
        m_pVectorStd->saveParam(p_pDoc, &nodeVector);
    }
    return bRet;
}

