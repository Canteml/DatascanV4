/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : CameraManager.cpp
# Author      : Thierry Lechevallier
# Date        : 09/11/2021
#
# ==========================================================================*/

// -- Internal includes
#include "camera/CameraManager.h"
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>

// -- Qt includes
#include <QTimer>
#include <QCameraInfo>

CameraManager* CameraManager::m_pMyInstance = nullptr;

CameraManager::CameraManager() : XmlSettingsLoggable("CameraManager")
{
    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        m_aCameraRole[i] = new CameraRole(i);
        m_aCameraRole[i]->setParent(this);
    }

    m_indexPreviewCamera = CAMERA_PREVIEW_UNDEF;
    m_roleMainCamera = 0;
    m_pVectorStd = nullptr;
    m_selectMainCameraAuto = true;

    m_pTimer = new QTimer();
    m_pTimer->setSingleShot(false);
    m_pTimer->setInterval(500);
    connect(m_pTimer, &QTimer::timeout, this, &CameraManager::onUpdateCameraList);

}

CameraManager::~CameraManager()
{
    stop();

    delete m_pTimer;

    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        delete m_aCameraRole[i];
        m_aCameraRole[i] = nullptr;
    }

    for(auto i : qAsConst(m_lCameraDatabase) ) {
        delete i;
    }
    m_lCameraDatabase.clear();

    for(auto i : qAsConst(m_lCameraVideo) ) {
        delete i;
    }
    m_lCameraVideo.clear();
}

CameraManager* CameraManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("JoystickManager::makeInstance instance already exists !");
    } else {
        m_pMyInstance = new CameraManager();
    }

    return m_pMyInstance;
}

CameraManager* CameraManager::getInstance()
{
    return m_pMyInstance;
}

void CameraManager::start()
{
    m_pVectorStd = VectorManager::getInstance()->getVectorStd();
    connect(m_pVectorStd, &VectorStd::prodIdChanged, this, &CameraManager::onVectorProductIdChanged);
    connect(m_pVectorStd, &VectorStd::evFuncStateConnectionChanged, this, &CameraManager::onUpdateCameraDatabase);
    connect(m_pVectorStd->getDeviceManager(), &ComDeviceManager::evDeviceListChanged, this, &CameraManager::onVectorDeviceListChanged);
    m_pTimer->start();
}

void CameraManager::stop()
{
    if( m_pTimer->isActive() ) {
        m_pTimer->stop();
    }
}

// -- Roles
CameraRole* CameraManager::camAlarm()
{
    return m_aCameraRole[CameraRole::ROLE_ALARM];
}

CameraRole* CameraManager::camSensor1()
{
    return m_aCameraRole[CameraRole::ROLE_SENSOR1];
}

CameraRole* CameraManager::camSensor2()
{
    return m_aCameraRole[CameraRole::ROLE_SENSOR2];
}

CameraRole* CameraManager::camSensor3()
{
    return m_aCameraRole[CameraRole::ROLE_SENSOR3];
}

CameraRole* CameraManager::camSensor4()
{
    return m_aCameraRole[CameraRole::ROLE_SENSOR4];
}

CameraRole* CameraManager::camFront()
{
    return m_aCameraRole[CameraRole::ROLE_FRONT];
}

CameraRole* CameraManager::camRear()
{
    return m_aCameraRole[CameraRole::ROLE_REAR];
}

// -- Cameras
//QCamera* CameraManager::getPreviewCamera() const
//{
//    if( 0 <= m_indexPreviewCamera && m_indexPreviewCamera < m_lCameraVideo.size()) {
////        fLogDebug1(QString("index pv = %1 / %2").arg(m_indexPreviewCamera).arg(m_lCameraVideo.size()));
////        fLogDebug1(QString("Cam state : %1 / status : %2").arg(m_lCameraVideo.at(m_indexPreviewCamera)->getCamera()->state())
////                   .arg(m_lCameraVideo.at(m_indexPreviewCamera)->getCamera()->status()));
//        return m_lCameraVideo.at(m_indexPreviewCamera)->getCamera();
//    }
//    return nullptr;
//}

qint32 CameraManager::getPreviewCameraIndex() const
{
    return m_indexPreviewCamera;
}

qint32 CameraManager::getMainCameraIndex() const
{
    return m_aCameraRole[m_roleMainCamera]->getCameraIndex();
}

void CameraManager::assignPreviewAs(qint32 p_role)
{
    // Map current preview camera with the specified role
    fLogDebug1(QString("index pv = %1/2").arg(m_indexPreviewCamera, m_lCameraVideo.size()));
    if( -1 != m_indexPreviewCamera ) {
        m_aCameraRole[p_role]->setCamera(m_lCameraVideo.at(m_indexPreviewCamera));
    }

    onUpdateCameraDatabase();

}

bool CameraManager::getMainCameraSelectAuto() const
{
    return m_selectMainCameraAuto;
}

void CameraManager::setMainCameraSelectAuto(bool p_value)
{
    if( m_selectMainCameraAuto != p_value ) {
        m_selectMainCameraAuto = p_value;
        emit selectMainCameraAutoChanged(m_selectMainCameraAuto);
    }
}

void CameraManager::setProductRoleMainCamera(qint32 p_productRole)
{
    // Key shortcut
    qint32 role = -1;
    qint32 pid = m_pVectorStd->getProdId();

    if( CameraRole::ROLE_ALARM == p_productRole ) {
        if( ProtMsg::PROD_ROVER == pid || ProtMsg::PROD_ROVER_CLEANER == pid ||  ProtMsg::PROD_PANDA_VOL1 == pid || ProtMsg::PROD_PANDA_VOL3 == pid ) {
            role = CameraRole::ROLE_ALARM;
        }
    } else if( 1 == p_productRole ) {
        if( ProtMsg::PROD_PANDA_VOL3 == pid ) {
            role = CameraRole::ROLE_SENSOR1;
        } else {
            role = CameraRole::ROLE_FRONT;
        }
    } else if( 2 == p_productRole ) {
        if(  ProtMsg::PROD_ALTISCAN == pid || ProtMsg::PROD_ROVER == pid ||  ProtMsg::PROD_ROVER_SCANNER == pid || ProtMsg::PROD_ROVER_CLEANER == pid ) {
            role = CameraRole::ROLE_REAR;
        } else if( ProtMsg::PROD_PANDA_VOL3 == pid ){
            role = CameraRole::ROLE_SENSOR2;
        }
    } else if( 3 == p_productRole ) {
        if( ProtMsg::PROD_PANDA_VOL3 == pid ) {
            role = CameraRole::ROLE_SENSOR3;
        }
    } else if( 4 == p_productRole ) {
        if( ProtMsg::PROD_PANDA_VOL3 == pid ) {
            role = CameraRole::ROLE_SENSOR3;
        }
    }

    if( -1 != role ) {
        setRoleMainCamera(role);
    }
}

void CameraManager::setRoleMainCamera(qint32 p_role)
{
    if( m_roleMainCamera != p_role ) {
        m_roleMainCamera = p_role;
        emit mainCameraIndexChanged(getMainCameraIndex());
    }
}

void CameraManager::setPreviewCameraIndex(qint32 p_index)
{
    if( m_indexPreviewCamera != p_index ) {
        m_indexPreviewCamera = p_index;
        //m_lCameraVideo.at(m_indexPreviewCamera)->capture("CAM");
        emit previewCameraIndexChanged(m_indexPreviewCamera);
    }
}

// -- SLOTS
void CameraManager::onVectorProductIdChanged(qint32 p_pid)
{
    // -- stop timer
    if( m_pTimer->isActive() ) {
        m_pTimer->stop();
    }

    // -- remove cameras from roles
    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        m_aCameraRole[i]->setCamera(nullptr);
    }

    // enable / disable roles
    if(p_pid == ProtMsg::PROD_ROVER || p_pid == ProtMsg::PROD_ROVER_CLEANER || p_pid == ProtMsg::PROD_PANDA_VOL1 || p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_ALARM]->setIsEnabled(true);
    } else {
        m_aCameraRole[CameraRole::ROLE_ALARM]->setIsEnabled(false);
    }

    if( p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_FRONT]->setIsEnabled(false);
    } else {
        m_aCameraRole[CameraRole::ROLE_FRONT]->setIsEnabled(true);
    }

    if(p_pid == ProtMsg::PROD_ALTISCAN || p_pid == ProtMsg::PROD_ROVER || p_pid == ProtMsg::PROD_ROVER_CLEANER || p_pid == ProtMsg::PROD_ROVER_SCANNER ) {
        m_aCameraRole[CameraRole::ROLE_REAR]->setIsEnabled(true);
    } else {
        m_aCameraRole[CameraRole::ROLE_REAR]->setIsEnabled(false);
    }

    if( p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_SENSOR1]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR2]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR3]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR4]->setIsEnabled(true);
    } else {
        m_aCameraRole[CameraRole::ROLE_SENSOR1]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR2]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR3]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR4]->setIsEnabled(false);
    }

    m_aCameraRole[CameraRole::ROLE_SENSOR4]->setIsEnabled(false);

    // -- Auto select the camera
    setMainCameraSelectAuto(p_pid == ProtMsg::PROD_PANDA_VOL3);

    onUpdateCameraDatabase();


    // -- restart timer
    m_pTimer->start();
}


void CameraManager::onVectorDeviceListChanged(qint32)
{
    // unmap camera in list of Role that are no more associated to a device

    // try to map a camera to a role function to list of CameraDatabase

}

void CameraManager::onUpdateCameraList()
{
    bool bListCameraChanged = false;
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    // -- Add new cameras
    qint32 index=0;
    // Parse all camera connected
    for (const QCameraInfo &cameraInfo : cameras) {
        bool bFound = false;
        // find if this camera is already instanciated
        for(auto i : qAsConst(m_lCameraVideo) ) {
            if( i->getCameraId() == cameraInfo.deviceName() ) {
                bFound = true;
                // update index if needed
                if( i->getCameraIndex() != index ) {
                    i->setCameraIndex(index);
                }
                break;
            }
        }
        if( !bFound ) {
            // camera not found => instanciate it
            CameraVideo * pCV = new CameraVideo(index, cameraInfo.deviceName());
            m_lCameraVideo.append(pCV);
            bListCameraChanged = true;
            // try to auto assign this camera
            for(auto db : qAsConst(m_lCameraDatabase) ) {
                if( db->getCameraId() == pCV->getCameraId() &&
                    db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid ) {
                    m_aCameraRole[db->getCameraRole()]->setCamera(pCV);
                }
            }
        }
        index++;
    }

    // -- remove camera no more connected
    for(auto i : qAsConst(m_lCameraVideo) ) {
        bool bFound = false;
        for (const QCameraInfo &cameraInfo : cameras) {
            if( i->getCameraId() == cameraInfo.deviceName() ) {
                bFound = true;
                break;
            }
        }
        if( !bFound ) {
            for(int r=0; r<CameraRole::ROLE_MAX; r++) {
                if( m_aCameraRole[r]->getCameraVideo() == i ) {
                    m_aCameraRole[r]->setCamera(nullptr);
                }
            }
            m_lCameraVideo.removeAll(i);
            delete i;
            bListCameraChanged = true;
        }
    }

    if( CAMERA_PREVIEW_UNDEF != m_indexPreviewCamera && m_lCameraVideo.isEmpty() ) {
        setPreviewCameraIndex(CAMERA_PREVIEW_UNDEF);
    } else {
        if( bListCameraChanged && 0 != m_indexPreviewCamera ) {
            setPreviewCameraIndex(0);
        }
    }

    if( bListCameraChanged ) {
        emit evCameraListChanged(m_lCameraVideo.size());
    }
}

void CameraManager::onUpdateCameraDatabase()
{
    // try to auto assign this camera
    for(auto c : qAsConst(m_lCameraVideo) ) {
        for(auto db : qAsConst(m_lCameraDatabase) ) {
            if( db->getCameraId() == c->getCameraId() &&
                db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid ) {
                if( ! m_aCameraRole[db->getCameraRole()]->getIsReady() ) {
                    m_aCameraRole[db->getCameraRole()]->setCamera(c);
                }
            }
        }
    }

    // update Database :
    for(int r=0; r<CameraRole::ROLE_MAX; r++) {
        // for all role
        if( m_aCameraRole[r]->getIsReady() ) {
            CameraDatabase* pCamDb = nullptr;
            for(auto i : qAsConst(m_lCameraDatabase)) {
                if( i->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid &&
                    i->getCameraRole() == r ) {
                    pCamDb = i;
                    break;
                }
            }
            if( nullptr == pCamDb ) {
                // new camera for product
                pCamDb = new CameraDatabase(m_lCameraVideo.at(m_indexPreviewCamera)->getCameraId(), m_pVectorStd->getFuncGeneralParam().m_paramDProdUid, r);
                m_lCameraDatabase.append(pCamDb);
            } else {
                // update product camera
                pCamDb->setCameraId(m_aCameraRole[r]->getCameraVideo()->getCameraId());
            }
        }
    }
}

// -- Settings
bool CameraManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;

    // load roles
    QDomElement eCamDbList = p_pParent->firstChildElement("CameraDatabaseList");
    if( eCamDbList.isNull() ) {
        fLogWarning("Xml element not found : CameraDataBaseList");
    } else {
        QDomElement eCamDB = eCamDbList.firstChildElement("CameraDatabase");
        while( !eCamDB.isNull() && bRet ) {
            // new role
            CameraDatabase* pCDB = new CameraDatabase();
            m_lCameraDatabase.append(pCDB);
            bRet &= pCDB->customLoadParam(&eCamDB);
            // next iteration
            eCamDB = eCamDB.nextSiblingElement("CameraDatabase");
        }
    }

    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_roleMainCamera, CameraRole::ROLE_ALARM);
    return bRet;
}

bool CameraManager::customLoadDefaultSettings()
{
    m_roleMainCamera = CameraRole::ROLE_ALARM;
    return true;
}

bool CameraManager::customApplySettings()
{
    emit previewCameraIndexChanged(m_indexPreviewCamera);
    emit mainCameraIndexChanged(getMainCameraIndex());
    return true;
}

bool CameraManager::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;        

    // save list CAmera Database
    QDomElement eList = p_pParent->firstChildElement("CameraDatabaseList");
    if( eList.isNull() ) {
        // Parameter doesn't exist, make it !
        eList = p_pDoc->createElement("CameraDatabaseList");
        p_pParent->appendChild(eList);
    } else {
        // Parameter found, change its value with a new element
        QDomNode nodes = eList.firstChild();
        while(eList.hasChildNodes())
        {
            eList.removeChild(nodes);
            nodes = eList.firstChild();
        }

    }


    for(auto i : qAsConst(m_lCameraDatabase)) {
        QDomElement eCamDb = p_pDoc->createElement(i->getXmlSectionName());
        eList.appendChild(eCamDb);
        i->customSaveParam(p_pDoc, &eCamDb);
    }

    bRet &= XML_WRITE_UNIQUE(m_roleMainCamera);
    return bRet;
}


