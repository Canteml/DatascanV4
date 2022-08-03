#include "LicenceManager.h"

#include <QTimer>
#include <QTime>

#include "CoreApp.h"

LicenceManager* LicenceManager::m_pMyInstance = nullptr;

LicenceManager::LicenceManager() : Loggable("LicenceManager")
{
    m_bLicenceIsValid = false;
    m_u32RemainingTime = 0u;
    m_u32RemainingDistance = 0u;
}

LicenceManager::~LicenceManager()
{
    m_pMyInstance = nullptr;
}

LicenceManager* LicenceManager::makeInstance()
{
    if(nullptr != m_pMyInstance) {
        Logger::logError("LicenceManager::makeInstance : manager already instanciated");
    } else {
        m_pMyInstance = new LicenceManager();
    }
    return m_pMyInstance;
}

LicenceManager* LicenceManager::getInstance()
{
    if(nullptr == m_pMyInstance) {
        Logger::logError("LicenceManager::getInstance : manager not instanciated");
    }
    return m_pMyInstance;
}

void LicenceManager::init()
{
    QTimer::singleShot(3000, this, &LicenceManager::setLicenceInvalid);
}

bool LicenceManager::getLicenceValidity()
{
    return m_bLicenceIsValid;
}

void LicenceManager::setLicenceValidity(bool p_bIsValid)
{
    m_bLicenceIsValid = p_bIsValid;
    emit evLicenceValidityChanged(m_bLicenceIsValid);
}

void LicenceManager::setLicenceValid()
{
    setLicenceValidity(true);
}

void LicenceManager::setLicenceInvalid()
{
    setLicenceValidity(false);
}

QString LicenceManager::getEquipmentKey()
{
    return m_qsEquipmentKey;
}

void LicenceManager::setEquipmentKey(QString p_qsEquipmentKey)
{
    m_qsEquipmentKey = p_qsEquipmentKey;
}
