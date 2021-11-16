/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Camera.cpp
# Author      : Thierry Lechevallier
# Date        : 09/11/2021
#
# ==========================================================================*/

// -- Internal includes
#include "camera/Camera.h"

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- CameraVideo

CameraVideo::CameraVideo(qint32 p_index, const QString& p_cameraId) : Loggable("CameraVideo")
{
    m_index = p_index;
    m_cameraId = p_cameraId;
}

CameraVideo::~CameraVideo()
{

}

// -- Parameter
QString CameraVideo::getCameraId() const
{
    return m_cameraId;
}

// -- Behaviour
qint32 CameraVideo::getCameraIndex() const
{
    return m_index;
}

void CameraVideo::setCameraIndex(qint32 p_index)
{
    m_index = p_index;
}

//QString CameraVideo::capture(const QString& p_prefix)
//{
//    QString fileName = QString("%1/%2 %3").arg(ExamManager::getInstance()->getExportFolder())
//                                .arg(p_prefix)
//                                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss.zzz"));
//    m_pCapture->capture(fileName);
//    return fileName;
//}

// ----------------------------------------------------------------------------
// -- CameraRole

CameraRole::CameraRole(qint32 p_role) : Loggable("CameraRole")
{
    m_role = p_role;
    m_isEnabled = false;
    m_isReady = false;
    m_pCamera = nullptr;
}

CameraRole::~CameraRole()
{
    m_pCamera = nullptr;
}

// -- Parameter
qint32 CameraRole::getCameraIndex() const
{
    if( nullptr == m_pCamera ) {
        return CameraVideo::CAMERA_INDEX_UNDEF;
    }
    return m_pCamera->getCameraIndex();
}

CameraVideo *CameraRole::getCameraVideo() const
{
    return m_pCamera;
}

void CameraRole::setCamera(CameraVideo* p_camera)
{
    m_pCamera = p_camera;
    updateReadyStatus();
    if( nullptr != m_pCamera ) {
        emit evCameraIndexChanged(m_pCamera->getCameraIndex());
    } else {
        emit evCameraIndexChanged(CameraVideo::CAMERA_INDEX_UNDEF);
    }
    emit evCameraVideoChanged(m_pCamera);
}

void CameraRole::setIsEnabled(bool p_value)
{
    if( m_isEnabled != p_value ) {
        m_isEnabled = p_value;
        emit evIsEnabledChanged(m_isEnabled);
        updateReadyStatus();
    }
}

bool CameraRole::getIsEnabled() const
{
    return m_isEnabled;
}

void CameraRole::setIsReady(bool p_value)
{
    if( m_isReady != p_value ) {
        m_isReady = p_value;
        emit evIsReadyChanged(m_isReady);
        emit evCameraIsReadyChanged(this, m_isReady);
    }
}

bool CameraRole::getIsReady() const
{
    return m_isReady;
}


qint32 CameraRole::getRole() const
{
    return m_role;
}

void CameraRole::updateReadyStatus()
{
    bool ready = false;
    if( nullptr != m_pCamera && m_isEnabled ) {
        ready = true;
    }
    setIsReady(ready);
}

// ----------------------------------------------------------------------------
// -- CameraDatabase

CameraDatabase::CameraDatabase() : Loggable("CameraDatabase"), XmlProject("CameraDatabase")
{
    m_cameraId = "";
    m_productId = "";
    m_role = 0;
}

CameraDatabase::CameraDatabase(const QString& p_cameraId, const QString& p_productUid, const qint32 p_role) : Loggable("CameraDatabase"), XmlProject("CameraDatabase")
{
    m_cameraId = p_cameraId;
    m_productId = p_productUid;
    m_role = p_role;
}

CameraDatabase::~CameraDatabase()
{

}

// -- Parameter
QString CameraDatabase::getCameraId() const
{
    return m_cameraId;
}

void CameraDatabase::setCameraId(const QString& p_value)
{
    m_cameraId = p_value;
}

QString CameraDatabase::getProductUid() const
{
    return m_productId;
}

void CameraDatabase::setProductUid(const QString& p_value)
{
    m_productId = p_value;
}

qint32 CameraDatabase::getCameraRole() const
{
    return m_role;
}

void CameraDatabase::setCameraRole(qint32 p_value)
{
    m_role = p_value;
}

// -- Settings
bool CameraDatabase::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_cameraId, "");
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_productId, "");
    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_role, 0);
    return bRet;
}

bool CameraDatabase::customLoadDefaultParam()
{
    m_cameraId = "";
    m_productId = "";
    m_role = 0;
    return true;
}

bool CameraDatabase::customApplyParam()
{
    return true;
}

bool CameraDatabase::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_cameraId);
    bRet &= XML_WRITE_UNIQUE(m_productId);
    bRet &= XML_WRITE_UNIQUE(m_role);
    return bRet;
}
