/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : GlobalManager.cpp
# Author      : Thierry Lechevallier
# Date        : 02/09/2021
#
# ==========================================================================*/

#include "GlobalManager.h"
#include <v2/exam/ExamManager.h>

GlobalManager* GlobalManager::m_pMyInstance = nullptr;

GlobalManager::GlobalManager() : Loggable("GlobalManager")
{
    m_guiPage = PAGE_SCAN;
    m_pExamManager = nullptr;
}

GlobalManager::~GlobalManager()
{
    m_pMyInstance = nullptr;
}

GlobalManager* GlobalManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("GlobalManager::makeInstance : manager already instanciated");
    } else {
        m_pMyInstance = new GlobalManager();
    }
    return m_pMyInstance;

}

GlobalManager* GlobalManager::getInstance()
{
    if( nullptr == m_pMyInstance ) {
        Logger::logError("ExamManager::getInstance : manager not instanciated");
    }
    return m_pMyInstance;
}

void GlobalManager::initManager()
{
    // get pointer of Managers
    m_pExamManager = ExamManager::getInstance();
    connect(m_pExamManager, &ExamManager::evExamStepChanged, this, &GlobalManager::onExamManagerStepChanged);
}

quint32 GlobalManager::getGuiPage() const
{
    return m_guiPage;
}

void GlobalManager::setGuiPage(quint32 p_value)
{
    if( m_guiPage != p_value ) {
        m_guiPage = p_value;
        emit evGuiPageChanged(m_guiPage);
    }
}

void GlobalManager::onExamManagerStepChanged(qint32)
{
    // each time user change step of BSCAN acquisition, go to scan page
    if( PAGE_SCAN != m_guiPage) {
        setGuiPage(PAGE_SCAN);
    }
}




