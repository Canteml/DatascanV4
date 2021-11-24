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
#include "CoreApp.h"
#include "GlobalManager.h"
#include <v2/exam/ExamManager.h>

// -- Qt includes
#include <QTimer>
#include <QCameraInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCamera>
#include <QThread>

CameraManager* CameraManager::m_pMyInstance = nullptr;

CameraManager::CameraManager() : XmlSettingsLoggable("CameraManager")
{
    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        m_aCameraRole[i] = new CameraRole(i);
        m_aCameraRole[i]->setParent(this);
        connect(m_aCameraRole[i], &CameraRole::evCameraIsReadyChanged, this, &CameraManager::onCameraIsReadyChanged);
    }

    m_slCameraRole.append(tr("Alarme"));
    m_slCameraRole.append(tr("Capteur 1"));
    m_slCameraRole.append(tr("Capteur 2"));
    m_slCameraRole.append(tr("Capteur 3"));
    m_slCameraRole.append(tr("Capteur 4"));
    m_slCameraRole.append(tr("Avant"));
    m_slCameraRole.append(tr("Arrière"));

    m_indexPreviewCamera = CAMERA_PREVIEW_UNDEF;
    m_roleMainCamera = 0;
    m_pVectorStd = nullptr;
    m_selectMainCameraAuto = false;
    m_mainCameraIsAlarm = false;

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
        Logger::logError("CameraManager::makeInstance instance already exists !");
    } else {
        m_pMyInstance = new CameraManager();
    }

    return m_pMyInstance;
}

CameraManager* CameraManager::getInstance()
{
    return m_pMyInstance;
}

void CameraManager::initQmlObject(QObject *p_pQmlTopLevel)
{
    QObject* qmlCamera = p_pQmlTopLevel->findChild<QObject*>("cameraAlarm");
    m_pCameraAlarm = qvariant_cast<QCamera*>(qmlCamera->property("mediaObject"));
    qmlCamera = p_pQmlTopLevel->findChild<QObject*>("cameraPreview");
    m_pCameraPreview = qvariant_cast<QCamera*>(qmlCamera->property("mediaObject"));
    qmlCamera = p_pQmlTopLevel->findChild<QObject*>("cameraMain");
    m_pCameraMain = qvariant_cast<QCamera*>(qmlCamera->property("mediaObject"));

    onGuiPageChanged(m_pGlobalManager->getGuiPage());
}


void CameraManager::onGuiPageChanged(quint32 p_page)
{
    if( 1 == p_page ) {
        // -- Configure : display on Preview video
        m_pCameraAlarm->stop();
        m_pCameraMain->stop();
        QThread::msleep(100);   ///< wait camera stop
        m_pCameraPreview->start();
        emit previewCameraIndexChanged(getPreviewCameraIndex());
    } else {
        // -- display on Alarm + Main video
        m_pCameraPreview->stop();
        QThread::msleep(100);   ///< wait camera stop
        m_pCameraMain->start();
        emit mainCameraIndexChanged(getMainCameraIndex());
        if( getMainCameraIndex() != getPreviewCameraIndex() ) {
            m_pCameraAlarm->start();
            emit mainCameraIndexChanged(getMainCameraIndex());
        }
    }
}

void CameraManager::onCameraIsReadyChanged(CameraRole *p_pCameraRole, bool p_enable)
{
    if( p_enable ) {
        if( -1 != m_roleMainCamera && p_pCameraRole == m_aCameraRole[m_roleMainCamera] ) {
            emit evMainCameraReady();
        }
        if( p_pCameraRole == m_aCameraRole[CameraRole::ROLE_ALARM] ) {
            if( m_aCameraRole[CameraRole::ROLE_ALARM]->getIsEnabled() && m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo() ) {
                if( getMainCameraId() != m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo()->getCameraId() ) {
                    if( 1 != m_pGlobalManager->getGuiPage() ) {
                        m_pCameraAlarm->start();
                    }
                }
            }
            emit evAlarmCameraReady();
        }
    }
}

void CameraManager::onExamControlTypeChanged(quint32 p_type)
{
    setMainCameraSelectAuto(p_type == ExamManager::CONTROL_PIPE);
}

void CameraManager::start()
{
    m_pGlobalManager = GlobalManager::getInstance();
    connect(m_pGlobalManager, &GlobalManager::evGuiPageChanged, this, &CameraManager::onGuiPageChanged);

    m_pExamManager = ExamManager::getInstance();
    connect(m_pExamManager, &ExamManager::evControlTypeChanged, this, &CameraManager::onExamControlTypeChanged);

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
qint32 CameraManager::getPreviewCameraIndex() const
{
    return m_indexPreviewCamera;
}

qint32 CameraManager::getMainCameraIndex() const
{
    return m_aCameraRole[m_roleMainCamera]->getCameraIndex();
}

QString CameraManager::getMainCameraId() const
{
    QString sRet;
    if( -1 != m_roleMainCamera && nullptr != m_aCameraRole[m_roleMainCamera]->getCameraVideo() ) {
        sRet = m_aCameraRole[m_roleMainCamera]->getCameraVideo()->getCameraId();
    }
    return sRet;
}

bool CameraManager::getMainCameraIsAlarm() const
{
    return m_mainCameraIsAlarm;
}

void CameraManager::assignPreviewAs(qint32 p_role)
{
    // Map current preview camera with the specified role
//    if( -1 != m_indexPreviewCamera ) {
//        m_aCameraRole[p_role]->setCamera(m_lCameraVideo.at(m_indexPreviewCamera));
//    }
    if( -1 != m_indexPreviewCamera ) {
        if(m_aCameraRole[p_role]->getCameraVideo() == m_lCameraVideo.at(m_indexPreviewCamera)) {
            // un-assign this camera => remove it from database
            for(auto db : qAsConst(m_lCameraDatabase) ) {
                if( db->getCameraRole() == p_role &&
                    db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid ) {
                        m_lCameraDatabase.removeAll(db);
                }
            }
            // camera already assigned for this role => remove assignation
            m_aCameraRole[p_role]->setCamera(nullptr);
        } else {
            // assign
            m_aCameraRole[p_role]->setCamera(m_lCameraVideo.at(m_indexPreviewCamera));
        }
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
    qint32 role = -1;

    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        if( m_aCameraRole[i]->getIsEnabled() ) {
            if( 0 == p_productRole ) {
                role = i;
                break;
            } else {
                p_productRole--;
            }
        }
    }

    if( -1 != role ) {
        setRoleMainCamera(role);
    }
}

void CameraManager::setProductRoleMainCameraNoAlarm(qint32 p_productRole)
{
    qint32 role = -1;

    for(int i=1; i<CameraRole::ROLE_MAX; i++) { ///< do not take care of alarm role
        if( m_aCameraRole[i]->getIsEnabled() ) {
            if( 0 == p_productRole ) {
                role = i;
                break;
            } else {
                p_productRole--;
            }
        }
    }

    if( -1 != role ) {
        setRoleMainCamera(role);
    }
}

qint32 CameraManager::getProductRoleMainCamera()
{
    qint32 role = 0;

    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        if( m_aCameraRole[i]->getIsEnabled() ) {
            if( i == m_roleMainCamera ) {
                break;
            }
            role++;
        }
    }

    return role;
}

void CameraManager::setRoleMainCamera(qint32 p_role)
{
    if( m_roleMainCamera != p_role ) {
        m_roleMainCamera = p_role;

        setMainCameraIsAlarm(m_roleMainCamera == CameraRole::ROLE_ALARM);

        if( m_aCameraRole[CameraRole::ROLE_ALARM]->getIsEnabled() && m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo() ) {
            if( getMainCameraId() == m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo()->getCameraId() ) {
                m_pCameraAlarm->stop();
            } else {
                m_pCameraAlarm->start();
            }
        }
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

QString CameraManager::captureMainCamera(const QString& p_prefix)
{
    QString fileName;
    if( -1 != m_roleMainCamera && m_aCameraRole[m_roleMainCamera]->getIsReady() ) {
        fileName = QString("%1/%2%3 %4.jpg").arg(ExamManager::getInstance()->getExportFolder())
                                    .arg(p_prefix)
                                    .arg(m_slCameraRole[m_roleMainCamera])
                                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss.zzz"));
        fileName.replace("/", "\\\\");
        emit evRequestMainCameraCapture(fileName);
    }
    return fileName;
}

void CameraManager::signalMainCameraCaptureSaved(QString p_fileName)
{
    emit evMainCameraCaptureSaved(p_fileName);
}

// -- SLOTS
void CameraManager::onVectorProductIdChanged(qint32 p_pid)
{
    // -- stop timer
    if( m_pTimer->isActive() ) {
        m_pTimer->stop();
    }

    QStringList slRoles;

    // -- remove cameras from roles
    for(int i=0; i<CameraRole::ROLE_MAX; i++) {
        m_aCameraRole[i]->setCamera(nullptr);
    }

    // enable / disable roles
    if(p_pid == ProtMsg::PROD_ROVER || p_pid == ProtMsg::PROD_ROVER_CLEANER || p_pid == ProtMsg::PROD_PANDA_VOL1 || p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_ALARM]->setIsEnabled(true);
        slRoles.append(tr("Alarme"));
    } else {
        m_aCameraRole[CameraRole::ROLE_ALARM]->setIsEnabled(false);
    }

    if( p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_FRONT]->setIsEnabled(false);
    } else {
        m_aCameraRole[CameraRole::ROLE_FRONT]->setIsEnabled(true);
        slRoles.append(tr("Avant"));
    }

    if(p_pid == ProtMsg::PROD_ALTISCAN || p_pid == ProtMsg::PROD_ROVER || p_pid == ProtMsg::PROD_ROVER_CLEANER || p_pid == ProtMsg::PROD_ROVER_SCANNER ) {
        m_aCameraRole[CameraRole::ROLE_REAR]->setIsEnabled(true);
        slRoles.append(tr("Arrière"));
    } else {
        m_aCameraRole[CameraRole::ROLE_REAR]->setIsEnabled(false);
    }

    if( p_pid == ProtMsg::PROD_PANDA_VOL3 ) {
        m_aCameraRole[CameraRole::ROLE_SENSOR1]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR2]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR3]->setIsEnabled(true);
        m_aCameraRole[CameraRole::ROLE_SENSOR4]->setIsEnabled(true);
        slRoles.append(tr("Capteur 1"));
        slRoles.append(tr("Capteur 2"));
        slRoles.append(tr("Capteur 3"));
        slRoles.append(tr("Capteur 4"));
    } else {
        m_aCameraRole[CameraRole::ROLE_SENSOR1]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR2]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR3]->setIsEnabled(false);
        m_aCameraRole[CameraRole::ROLE_SENSOR4]->setIsEnabled(false);
    }

    // -- Auto select the camera
    onUpdateCameraDatabase();

    CoreApp::getInstanceCoreUiQml()->getApplicationEngine()->rootContext()->setContextProperty("modelCameraRoleList", QVariant::fromValue(slRoles));

    if( !getMainCameraSelectAuto() ) {
        setProductRoleMainCameraNoAlarm(0);
    }

    // -- restart timer
    m_pTimer->start();
}


void CameraManager::onVectorDeviceListChanged(qint32)
{
    // unmap camera in list of Role that are no more associated to a device

    // try to map a camera to a role function to list of CameraDatabase
    onUpdateCameraDatabase();

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
    QList<CameraVideo* > camToDelete;
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
            camToDelete.append(i);
            bListCameraChanged = true;
        }
    }    
    for(auto i : qAsConst(camToDelete)) {
        m_lCameraVideo.removeAll(i);
        delete i;
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

    return bRet;
}

void CameraManager::setMainCameraIsAlarm(bool p_value)
{
    if( m_mainCameraIsAlarm != p_value) {
        m_mainCameraIsAlarm = p_value;
        emit mainCameraIsAlarmChanged(m_mainCameraIsAlarm);
    }
}


