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
#include "CameraManager.h"
#include "GlobalManager.h"
#include "CoreApp.h"
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/exam/ExamManager.h>
#include "camera/VlcQmlVideoSink.h"

// -- Qt includes
#include <QTimer>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCamera>
#include <QThread>
#include <QFile>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Video.h>
#include <camera/VlcVideoSink.h>

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

    QStringList vlcArgs = QStringList(QString("--network-caching=300"));
    vlcArgs.append("--clock-synchro=0");        // Clock synchronisation  It is possible to disable the input clock synchronisation for  real-time sources. Use this if you experience jerky playback of network streams.
    //vlcArgs.append("--clock-jitter=100");     // This defines the maximum input delay jitter that the synchronization  algorithms should try to compensate (in milliseconds).
    //vlcArgs.append("--live-caching=300");       // Live capture caching (ms)  Caching value for cameras and microphones, in milliseconds.
    //vlcArgs.append("--no-ts-trust-pcr");      //
    //vlcArgs.append("--no-avcodec-fast");        // --avcodec-fast, --no-avcodec-fast
    // Allow speed tricks
    // (default disabled)
    // Allow non specification compliant speedup tricks. Faster but
    // error-prone.
    //vlcArgs.append("-vvv");
    //vlcArgs.append("--sout-transcode-venc=x265");
    vlcArgs.append("--quiet");                  // Console logger : (console) -q, --quiet, --no-quiet        Be quiet(default disabled)

    m_pInstance = new VlcInstance(vlcArgs, this);
    m_pMainPlayer = new VlcMediaPlayer(m_pInstance);
    m_pPreviewPlayer = new VlcMediaPlayer(m_pInstance);

    for(int i = IP_CAMERA_MIN ; i <= IP_CAMERA_MAX ; i++) {
        QString sSuffix = QString::number(i);
        QString sAddress = QString("192.168.0.") + sSuffix;
        m_lIpCamera.append(new IpCamera(QString("IP Camera ." + sSuffix), sAddress, m_pInstance));
    }

    m_indexPreviewCamera = CAMERA_PREVIEW_UNDEF;
    m_roleMainCamera = 0;
    m_pVectorStd = nullptr;
    m_selectMainCameraAuto = false;
    m_mainCameraIsAlarm = false;

    m_pMainSink    = new VlcVideoSink(m_pMainPlayer, this);
    m_pPreviewSink = new VlcVideoSink(m_pPreviewPlayer, this);


    m_pTimer = new QTimer();
    m_pTimer->setSingleShot(false); // You can set a timer to time out only once by calling setSingleShot(true). You can also use the static QTimer::singleShot() function to call a slot after a specified interval:
    m_pTimer->setInterval(500);     // replace by m_pTimer->start(500) ? (TD)
    connect(m_pTimer, &QTimer::timeout, this, &CameraManager::onUpdateCameraList);
    connect(this, &CameraManager::mainCameraIndexChanged, this, &CameraManager::onMainCameraIndexChanged);

    m_pTimerRefreshMainCam =  new QTimer();
    m_pTimerRefreshMainCam->setSingleShot(false); // You can set a timer to time out only once by calling setSingleShot(true). You can also use the static QTimer::singleShot() function to call a slot after a specified interval:
    m_pTimerRefreshMainCam->setInterval(1000);     // replace by m_pTimer->start(500) ? (TD)
    connect(m_pTimerRefreshMainCam, &QTimer::timeout, this, &CameraManager::onRefreshCameraAutoReco);

    m_pCameraAlarm   = new QCamera(this);
    m_pCameraMain    = new QCamera(this);
    m_pCameraPreview = new QCamera(this);


    m_pSessionAlarm   = new QMediaCaptureSession(this);
    m_pSessionMain    = new QMediaCaptureSession(this);
    m_pSessionPreview = new QMediaCaptureSession(this);


    m_pSessionAlarm->setCamera(m_pCameraAlarm);
    m_pSessionMain->setCamera(m_pCameraMain);
    m_pSessionPreview->setCamera(m_pCameraPreview);

}

QObject* CameraManager::getPreviewCameraSource() const {
    return m_pPreviewPlayer;
}

CameraManager::~CameraManager()
{
    stop();

    // Prima disconnetti i sink per evitare callback su puntatori invalidi
    if (m_pPreviewSink) m_pPreviewSink->setVideoSink(nullptr);
    if (m_pMainSink) m_pMainSink->setVideoSink(nullptr);

    // Poi ferma i player
    if (m_pPreviewPlayer) m_pPreviewPlayer->stop();
    if (m_pMainPlayer) m_pMainPlayer->stop();
    QThread::msleep(300);

    delete m_pTimer;
    delete m_pTimerRefreshMainCam;

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

    if (m_pMainPlayer) m_pMainPlayer->stop();
    if (m_pPreviewPlayer) m_pPreviewPlayer->stop();

    for(auto i : qAsConst(m_lIpCamera)) {
        delete i;
    }
    m_lIpCamera.clear();


    delete m_pMainPlayer;
    delete m_pPreviewPlayer;
    delete m_pInstance;
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
    m_pQmlTopLevel = p_pQmlTopLevel;
    QStringList cameraNames;
    cameraNames.append(tr("Caméra 1 (USB)"));
    CoreApp::getInstanceCoreUiQml()
        ->getApplicationEngine()
        ->rootContext()
        ->setContextProperty("modelCameraList", QVariant::fromValue(cameraNames));
    onGuiPageChanged(m_pGlobalManager->getGuiPage());
}

void CameraManager::onGuiPageChanged(quint32 p_page)
{

    /*
    if( 1 == p_page ) {
        if (m_pCameraAlarm) m_pCameraAlarm->stop();
        if (m_pCameraMain) m_pCameraMain->stop();

        QTimer::singleShot(3000, this, [this]() {
            QObject* vo = m_pQmlTopLevel->findChild<QObject*>(
                "previewVideoOutput", Qt::FindChildrenRecursively);
            FILE* f = fopen("C:/ProgramData/Roboplanet/DatascanV3/conf/camera_test.txt", "a");
            if (f) { fprintf(f, "findChild previewVideoOutput: %s\n", vo ? "TROVATO" : "NON TROVATO"); fclose(f); }
            if (vo) {
                setPreviewVideoSink(vo);
            } else {
                // Riprova ogni secondo finché non trovato
                QTimer::singleShot(1000, this, [this]() {
                    QObject* vo2 = m_pQmlTopLevel->findChild<QObject*>(
                        "previewVideoOutput", Qt::FindChildrenRecursively);
                    FILE* f2 = fopen("C:/ProgramData/Roboplanet/DatascanV3/conf/camera_test.txt", "a");
                    if (f2) { fprintf(f2, "retry findChild: %s\n", vo2 ? "TROVATO" : "NON TROVATO"); fclose(f2); }
                    if (vo2) setPreviewVideoSink(vo2);
                });
            }
        });

        emit previewCameraIndexChanged(getPreviewCameraIndex());
    }*/
    if( 1 == p_page ) {
        if (m_pCameraAlarm) m_pCameraAlarm->stop();
        if (m_pCameraMain) m_pCameraMain->stop();

        // Solo se il sink non è già connesso
        if (!m_pPreviewQVideoSink) {
            QTimer::singleShot(3000, this, [this]() {
                QObject* vo = m_pQmlTopLevel->findChild<QObject*>(
                    "previewVideoOutput", Qt::FindChildrenRecursively);
                if (vo) {
                    setPreviewVideoSink(vo);
                } else {
                    QTimer::singleShot(1000, this, [this]() {
                        QObject* vo2 = m_pQmlTopLevel->findChild<QObject*>(
                            "previewVideoOutput", Qt::FindChildrenRecursively);
                        if (vo2) setPreviewVideoSink(vo2);
                    });
                }
            });
        }

        emit previewCameraIndexChanged(getPreviewCameraIndex());
    }
    else if( 3 == p_page ) // page Scan
    {

        if (m_pCameraPreview) m_pCameraPreview->stop();

        //QThread::msleep(500);   ///< wait camera stop
        /*
        if(isMainCameraIp()) {
            openMainIpMedia();
        }
        */

        emit mainCameraIndexChanged(getMainCameraIndex());

    }
    else {

        if (m_pCameraAlarm) m_pCameraAlarm->stop();
        if (m_pCameraMain) m_pCameraMain->stop();
        if (m_pCameraPreview) m_pCameraPreview->stop();

        if (m_pPreviewPlayer) m_pPreviewPlayer->stop();
        if (m_pMainPlayer) m_pMainPlayer->stop();

        QThread::msleep(300);
    }
}

void CameraManager::onCameraIsReadyChanged(CameraRole *p_pCameraRole, bool p_enable)
{
    if( p_enable ) {
        if(isMainCameraIp()) {
            openMainIpMedia();
            emit isMainCameraIpChanged(true);
        }
        if( -1 != m_roleMainCamera && p_pCameraRole == m_aCameraRole[m_roleMainCamera] ) {
            if( !isMainCameraIp() ) {
                startMainUsbCamera();
            }
            emit evMainCameraReady();
        }
        if( p_pCameraRole == m_aCameraRole[CameraRole::ROLE_ALARM] ) {
            if( m_aCameraRole[CameraRole::ROLE_ALARM]->getIsEnabled() && m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo() ) {
                if( getMainCameraId() != m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo()->getCameraId() ) {
                    if( 1 != m_pGlobalManager->getGuiPage() ) {
                        if (m_pCameraAlarm)  m_pCameraAlarm->start();
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

void CameraManager::onMainCameraIndexChanged(quint32 p_index)
{
    if(isMainCameraIp()) {
        if (m_pCameraMain) m_pCameraMain->stop();
        openMainIpMedia();
    } else {
        startMainUsbCamera();
    }
}

void CameraManager::startMainUsbCamera()
{
    if (!m_pCameraMain || !m_pGlobalManager) return;

    // Main camera is displayed only on page Scan
    if (3 != m_pGlobalManager->getGuiPage()) return;

    CameraVideo* pCamVideo = m_aCameraRole[m_roleMainCamera]->getCameraVideo();
    if (!m_aCameraRole[m_roleMainCamera]->getIsReady() || !pCamVideo || pCamVideo->isIpCamera()) {
        m_pCameraMain->stop();
        return;
    }

    const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    for (const QCameraDevice& d : devices) {
        if (d.id() == pCamVideo->getCameraId()) {
            if (m_pMainPlayer) m_pMainPlayer->stop();
            if (m_pCameraPreview) m_pCameraPreview->stop();   // USB device can not be opened twice
            if (m_pCameraMain->cameraDevice() != d) {
                m_pCameraMain->stop();
                m_pCameraMain->setCameraDevice(d);
            }
            m_pCameraMain->start();
            return;
        }
    }
}

void CameraManager::setPreviewVlcSink(QObject* vlcSinkObj)
{

    VlcQmlVideoSink* vlcSink = qobject_cast<VlcQmlVideoSink*>(vlcSinkObj);
    if (vlcSink && vlcSink->videoSink()) {
        m_pPreviewQVideoSink = vlcSink->videoSink();
        m_pPreviewSink->setVideoSink(m_pPreviewQVideoSink);
        if (isPreviewCameraIp() && m_indexPreviewCamera != CAMERA_PREVIEW_UNDEF) {
            openPreviewIpMedia();
        }
    }
}

void CameraManager::onRefreshCameraAutoReco()
{
    QString     strFilePath = CoreApp::getDirDataLog() + QString("/") + "log.txt";
    QFile       logFile(strFilePath);
    QStringList stringList;
    QByteArray  qbaContent;
    int         s16LineMax=0;
    int         s16LineStart=0;
    int         s16Idx;
    bool        bFoundVLClibError=false;

    if(logFile.open(QIODevice::ReadOnly | QIODevice::Text))//if there is a log
    {
        QTextStream textStream(&logFile);
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                stringList.append(line);
        }
        s16LineMax = stringList.size();

        if(0> (s16LineMax-4))
        {
            s16LineStart = 0;
        }
        else
        {
            s16LineStart = s16LineMax-4;
        }

        //read the 4last line
        for(s16Idx = s16LineStart;s16Idx< s16LineMax ; s16Idx++   )
        {
            //qDebug() << stringList[s16Idx];
            bFoundVLClibError |= stringList[s16Idx].contains("- Qt : libvlc: more than 5 seconds of late video -> dropping frame (computer too slow ?)");
            bFoundVLClibError |= stringList[s16Idx].contains("- Qt : libvlc: ES_OUT_SET_(GROUP_)PCR  is called too late");
        }
    }

    if(true== bFoundVLClibError)
    {
        qDebug("Freez Camera detected find ");

        if (m_pCameraAlarm) m_pCameraAlarm->stop();
        if (m_pCameraMain) m_pCameraMain->stop();
        if (m_pCameraPreview) m_pCameraPreview->stop();

        //QThread::msleep(300);   ///< wait camera stop

        //m_pPreviewPlayer->stop();
        //m_pMainPlayer->stop();


        emit previewCameraIndexChanged(getMainCameraIndex());
        emit mainCameraIndexChanged(getMainCameraIndex());
    }



}

void CameraManager::start()
{
    QStringList cameraNames;
    cameraNames.append(tr("Caméra 1 (USB)"));
    CoreApp::getInstanceCoreUiQml()
        ->getApplicationEngine()
        ->rootContext()
        ->setContextProperty("modelCameraList", QVariant::fromValue(cameraNames));

    m_pGlobalManager = GlobalManager::getInstance();

    //const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    m_pGlobalManager = GlobalManager::getInstance();
    connect(m_pGlobalManager, &GlobalManager::evGuiPageChanged, this, &CameraManager::onGuiPageChanged);

    m_pExamManager = ExamManager::getInstance();
    connect(m_pExamManager, &ExamManager::evControlTypeChanged, this, &CameraManager::onExamControlTypeChanged);

    m_pVectorStd = VectorManager::getInstance()->getVectorStd();
    connect(m_pVectorStd, &VectorStd::prodIdChanged, this, &CameraManager::onVectorProductIdChanged);
    connect(m_pVectorStd, &VectorStd::evFuncStateConnectionChanged, this, &CameraManager::onUpdateCameraDatabase);
    connect(m_pVectorStd->getDeviceManager(), &ComDeviceManager::evDeviceListChanged, this, &CameraManager::onVectorDeviceListChanged);
    m_pTimer->start();
    m_pTimerRefreshMainCam->start();
}

void CameraManager::connectPreviewSink(QObject* videoOutput)
{

    if (!videoOutput) return;
    QVideoSink* sink = videoOutput->property("videoSink").value<QVideoSink*>();

    if (sink) {
        m_pPreviewSink->setVideoSink(sink);
    }
}

void CameraManager::connectMainSink(QObject* videoOutput)
{
    if (!videoOutput) return;
    QVideoSink* sink = videoOutput->property("videoSink").value<QVideoSink*>();
    if (!sink || !m_pSessionMain) return;
    if (sink) {
        m_pSessionMain->setVideoSink(sink);
    }
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

/*
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
*/

void CameraManager::assignPreviewAs(qint32 p_role)
{
    // Trova la camera USB (indice 0) o IP (indice 1)
    CameraVideo* pCamVideo = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == m_indexPreviewCamera) {
            pCamVideo = cam;
            break;
        }
    }
    // Per indice 0 (USB), cerca la prima non-IP se non trovata per indice
    if (!pCamVideo && m_indexPreviewCamera == 0) {
        for (auto cam : qAsConst(m_lCameraVideo)) {
            if (!cam->isIpCamera()) {
                pCamVideo = cam;
                break;
            }
        }
    }
    if (!pCamVideo) return;

    if (m_aCameraRole[p_role]->getCameraVideo() == pCamVideo) {
        for (auto db : qAsConst(m_lCameraDatabase)) {
            if (db->getCameraRole() == p_role &&
                db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid) {
                m_lCameraDatabase.removeAll(db);
            }
        }
        m_aCameraRole[p_role]->setCamera(nullptr);
    } else {
        m_aCameraRole[p_role]->setCamera(pCamVideo);
    }
    onUpdateCameraDatabase();
}

void CameraManager::assignCameraToRole(qint32 p_role, qint32 p_cameraIndex)
{
    if (p_role < 0 || p_role >= CameraRole::ROLE_MAX) return;

    CameraVideo* pCamVideo = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == p_cameraIndex) {
            pCamVideo = cam;
            break;
        }
    }

    if (m_aCameraRole[p_role]->getCameraVideo() == pCamVideo) return;

    // Remove the previous assignation from database, otherwise onUpdateCameraDatabase() restores it
    if (m_pVectorStd) {
        QList<CameraDatabase*> dbToDelete;
        for (auto db : qAsConst(m_lCameraDatabase)) {
            if (db->getCameraRole() == p_role &&
                db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid) {
                dbToDelete.append(db);
            }
        }
        for (auto db : qAsConst(dbToDelete)) {
            m_lCameraDatabase.removeAll(db);
            delete db;
        }
    }

    // nullptr (index -1 or not found) => un-assign
    m_aCameraRole[p_role]->setCamera(pCamVideo);
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
        bool wasIp = isMainCameraIp();
        m_roleMainCamera = p_role;
        bool isIp = isMainCameraIp();

        setMainCameraIsAlarm(m_roleMainCamera == CameraRole::ROLE_ALARM);

        if(wasIp != isIp) {
            emit isMainCameraIpChanged(isIp);
        }

        if( m_aCameraRole[CameraRole::ROLE_ALARM]->getIsEnabled() && m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo() ) {
            if( getMainCameraId() == m_aCameraRole[CameraRole::ROLE_ALARM]->getCameraVideo()->getCameraId() ) {
                if (m_pCameraAlarm) m_pCameraAlarm->stop();
            } else {
                if (m_pCameraAlarm)  m_pCameraAlarm->start();
            }
        }
        emit mainCameraIndexChanged(getMainCameraIndex());
    }
}

void CameraManager::setPreviewVideoSink(QObject* videoOutput)
{
    if (!videoOutput) return;
    QVideoSink* sink = videoOutput->property("videoSink").value<QVideoSink*>();

    if (!sink) {
        QTimer::singleShot(100, this, [this, videoOutput]() {
            setPreviewVideoSink(videoOutput);
        });
        return;
    }

    m_pPreviewQVideoSink = sink;
    m_pSessionPreview->setVideoSink(sink);
    m_pPreviewSink->setVideoSink(sink);

    if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF || m_indexPreviewCamera == 0) {
        startUsbPreview(sink);
        if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF) {
            m_indexPreviewCamera = 0;
            emit previewCameraIndexChanged(0);
        }
    } else {
        // Camera IP già selezionata — ricollegare il sink e riavviare VLC
        openPreviewIpMedia();
    }

    /*
    // Avvia sempre la camera USB (indice 0)
    startUsbPreview(sink);

    if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF) {
        m_indexPreviewCamera = 0;
        emit previewCameraIndexChanged(0);
    }
*/
}

void CameraManager::startUsbPreview(QVideoSink* sink)
{
    if (!m_pCameraPreview) return;

    const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    if (devices.isEmpty()) return;

    // Cerca il device USB nella lista m_lCameraVideo
    // Se non c'è nessuna camera reale, usa il primo device disponibile (OBS)
    QCameraDevice deviceToUse = devices.first();
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (!cam->isIpCamera()) {
            for (const QCameraDevice& d : devices) {
                if (d.id() == cam->getCameraId()) {
                    deviceToUse = d;
                    break;
                }
            }
            break;
        }
    }

    m_pPreviewPlayer->stop();  // ferma VLC se attivo
    m_pCameraPreview->setCameraDevice(deviceToUse);
    if (sink) m_pSessionPreview->setVideoSink(sink);
    m_pCameraPreview->start();
}

//////////////////////////////////
/*
void CameraManager::setPreviewVideoSink(QObject* videoOutput)
{
    if (!videoOutput) return;
    QVideoSink* sink = videoOutput->property("videoSink").value<QVideoSink*>();

    if (!sink) {
        // Il VideoOutput esiste ma il sink non è ancora pronto: riprova
        QTimer::singleShot(100, this, [this, videoOutput]() {
            setPreviewVideoSink(videoOutput);
        });

        return;
    }

    m_pPreviewQVideoSink = sink;

    if (m_pSessionPreview) {
        m_pSessionPreview->setVideoSink(sink);
    }
    if (m_pPreviewSink) {
        m_pPreviewSink->setVideoSink(sink);
    }

    if (m_pCameraPreview) {
        // Se non c'è un device specifico impostato, Qt usa il default (primo disponibile)
        m_pSessionPreview->setVideoSink(sink);
        m_pCameraPreview->start();
        if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF) {
            m_indexPreviewCamera = 0;
            emit previewCameraIndexChanged(0);
        }
    }
}
*/

void CameraManager::setPreviewCameraIndex(qint32 p_index)
{
    if (m_indexPreviewCamera != p_index) {
        m_indexPreviewCamera = p_index;

        if (p_index == CAMERA_PREVIEW_UNDEF) {
            m_pPreviewPlayer->stop();
            if (m_pCameraPreview) m_pCameraPreview->stop();
        } else if (p_index == 0) {
            // SEMPRE USB — indipendentemente da cosa c'è nella lista
            m_pPreviewPlayer->stop();
            startUsbPreview(m_pPreviewQVideoSink);
        } else {
            // Indice 1+ → IP
            if (m_pCameraPreview) m_pCameraPreview->stop();
            openPreviewIpMedia();
        }

        emit isPreviewCameraIpChanged(p_index != 0 && p_index != CAMERA_PREVIEW_UNDEF);
        emit previewCameraIndexChanged(m_indexPreviewCamera);
    }
}

/*
void CameraManager::setPreviewCameraIndex(qint32 p_index)
{
    if( m_indexPreviewCamera != p_index ) {
        bool bWasIp = isPreviewCameraIp();
        m_indexPreviewCamera = p_index;
        bool isIp = isPreviewCameraIp();
        if(bWasIp != isIp) {
            emit isPreviewCameraIpChanged(isIp);
        }
        if (p_index == CAMERA_PREVIEW_UNDEF) {
            // Stoppa tutto
            m_pPreviewPlayer->stop();
            if (m_pCameraPreview) m_pCameraPreview->stop();
        } else if (isIp) {
            // Stoppa USB, avvia VLC
            if (m_pCameraPreview) m_pCameraPreview->stop();
            openPreviewIpMedia();
        } else {
            // USB: stoppa VLC, imposta device, avvia
            m_pPreviewPlayer->stop();
            startUsbPreview(m_pPreviewQVideoSink);

            if (m_pCameraPreview) {
                // Trova il device USB corrispondente all'indice
                const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
                for (auto cam : qAsConst(m_lCameraVideo)) {
                    if (cam->getCameraIndex() == p_index && !cam->isIpCamera()) {
                        // Cerca il QCameraDevice corrispondente
                        for (const QCameraDevice& d : devices) {
                            if (d.id() == cam->getCameraId()) {
                                m_pCameraPreview->setCameraDevice(d);
                                if (m_pPreviewQVideoSink) {
                                    m_pSessionPreview->setVideoSink(m_pPreviewQVideoSink);
                                }
                                m_pCameraPreview->start();
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        emit previewCameraIndexChanged(m_indexPreviewCamera);
    }
}
*/

QString CameraManager::captureMainCamera(const QString& p_prefix)
{
    QString fileName;
    if( -1 != m_roleMainCamera && m_aCameraRole[m_roleMainCamera]->getIsReady() ) {
        fileName = QString("%1/%2%3 %4.jpg").arg(ExamManager::getInstance()->getExportFolder())
        .arg(p_prefix)
            .arg(m_slCameraRole[m_roleMainCamera])
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss.zzz"));
        fileName.replace("/", "\\\\");
        if(isMainCameraIp()) {
            m_pMainPlayer->video()->takeSnapshot(fileName);
            emit evMainCameraCaptureSaved(fileName);
        } else {
            emit evRequestMainCameraCapture(fileName);
        }
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
    if(p_pid == ProtMsg::PROD_ROVER || p_pid == ProtMsg::PROD_PANDA_VOL3 || p_pid == ProtMsg::PROD_MULTISCAN ) {
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

    if(/*p_pid == ProtMsg::PROD_ALTISCAN_V2 ||*/ p_pid == ProtMsg::PROD_ROVER
        || p_pid == ProtMsg::PROD_ROVER_IP68 || p_pid == ProtMsg::PROD_MULTISCAN) {
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

/*
void CameraManager::onUpdateCameraList()
{
    bool bListCameraChanged = false;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();

    // -- Add new cameras
    qint32 index=0;
    // Parse all camera connected
    for (const QCameraDevice &cameraInfo : cameras) {
        bool bFound = false;
        // find if this camera is already instanciated
        for(auto i : qAsConst(m_lCameraVideo) ) {
            if( i->getCameraId() == cameraInfo.id() ) {
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
            CameraVideo * pCV = new CameraVideo(index, cameraInfo.id());
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

        if (bListCameraChanged) {
            // Costruisce la lista nomi per il ComboBox QML
            QStringList cameraNames;
            const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
            for (const QCameraDevice& d : devices) {
                cameraNames.append(d.description());
            }
            fLogInfo(QString("Camera trovate: %1 -> %2").arg(devices.size()).arg(cameraNames.join(", ")));

            CoreApp::getInstanceCoreUiQml()
                ->getApplicationEngine()
                ->rootContext()
                ->setContextProperty("modelCameraList", QVariant::fromValue(cameraNames));

            emit evCameraListChanged(m_lCameraVideo.size());
        }
    }

    for (IpCamera* pIpCam : m_lIpCamera) {
        pIpCam->refreshStatus();
        if(pIpCam->isOnline()) {
            bool bFound = false;
            for(auto i : qAsConst(m_lCameraVideo)) {
                if(i->getCameraId() == pIpCam->getCameraId()) {
                    bFound = true;
                    // update index if needed
                    if( i->getCameraIndex() != index ) {
                        i->setCameraIndex(index);
                    }
                    break;
                }
            }
            if(!bFound) {
                IpCamera* pCamToAdd = new IpCamera(pIpCam->getCameraId(), pIpCam->ipAddress(), m_pInstance);
                pCamToAdd->setCameraIndex(index);
                bListCameraChanged = true;
                m_lCameraVideo.append(pCamToAdd);
                for(auto db : qAsConst(m_lCameraDatabase) ) {
                    if( db->getCameraId() == pCamToAdd->getCameraId() &&
                        db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid ) {
                        m_aCameraRole[db->getCameraRole()]->setCamera(pCamToAdd);
                    }
                }
            }
            index++;
        }
    }

    // -- remove camera no more connected
    QList<CameraVideo* > camToDelete;
    for(auto i : qAsConst(m_lCameraVideo) ) {
        bool bFound = false;
        for (const QCameraDevice &cameraInfo : cameras) {
            if( i->getCameraId() == cameraInfo.id() ) {
                bFound = true;
                break;
            }
        }
        for (IpCamera* pIpCam : m_lIpCamera) {
            if(pIpCam->isOnline() && pIpCam->getCameraId() == i->getCameraId()) {
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

    if( m_lCameraVideo.isEmpty() ) {
        if( CAMERA_PREVIEW_UNDEF != m_indexPreviewCamera ) {
            setPreviewCameraIndex(CAMERA_PREVIEW_UNDEF);
        }
    } else {
        if( bListCameraChanged ) {
            // Aggiorna solo la lista nella ComboBox, NON avviare la camera
            emit evCameraListChanged(m_lCameraVideo.size());
        }
    }
}
*/
void CameraManager::onUpdateCameraList()
{
    bool bListCameraChanged = false;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();

    // Costruisci una nuova lista ordinata: indice 0 = USB, indice 1 = IP
    QList<CameraVideo*> newList;

    // --- INDICE 0: prima camera USB ---
    if (!cameras.isEmpty()) {
        const QCameraDevice& cam = cameras.first();
        // cerca se esiste già
        CameraVideo* pExisting = nullptr;
        for (auto i : qAsConst(m_lCameraVideo)) {
            if (!i->isIpCamera() && i->getCameraId() == cam.id()) {
                pExisting = i;
                break;
            }
        }
        if (!pExisting) {
            pExisting = new CameraVideo(0, cam.id());
            bListCameraChanged = true;
            for (auto db : qAsConst(m_lCameraDatabase)) {
                if (db->getCameraId() == pExisting->getCameraId() &&
                    db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid) {
                    m_aCameraRole[db->getCameraRole()]->setCamera(pExisting);
                }
            }
        }
        pExisting->setCameraIndex(0);
        newList.append(pExisting);
    }

    // --- INDICE 1: prima IP online ---
    bool ipOnline = false;
    for (IpCamera* pIpCam : m_lIpCamera) {
        pIpCam->refreshStatus();
        if (pIpCam->isOnline()) {
            ipOnline = true;
            CameraVideo* pExisting = nullptr;
            for (auto i : qAsConst(m_lCameraVideo)) {
                if (i->isIpCamera() && i->getCameraId() == pIpCam->getCameraId()) {
                    pExisting = i;
                    break;
                }
            }
            if (!pExisting) {
                IpCamera* pNew = new IpCamera(pIpCam->getCameraId(), pIpCam->ipAddress(), m_pInstance);
                pExisting = pNew;
                bListCameraChanged = true;
            }
            // L'indice nella lista è la dimensione corrente di newList
            pExisting->setCameraIndex(newList.size());
            newList.append(pExisting);
            break;
        }
    }

    // --- Elimina le camera rimosse ---
    for (auto i : qAsConst(m_lCameraVideo)) {
        if (!newList.contains(i)) {
            for (int r = 0; r < CameraRole::ROLE_MAX; r++) {
                if (m_aCameraRole[r]->getCameraVideo() == i)
                    m_aCameraRole[r]->setCamera(nullptr);
            }
            bListCameraChanged = true;
            delete i;
        }
    }

    /////////////////////////////////////////////
    // Sostituisci la lista
    m_lCameraVideo = newList;

    // Aggiorna ComboBox SEMPRE
    QStringList cameraNames;
    cameraNames.append(tr("Caméra 1 (USB)"));
    if (ipOnline)
        cameraNames.append(tr("Caméra 2 (IP)"));
    CoreApp::getInstanceCoreUiQml()
        ->getApplicationEngine()
        ->rootContext()
        ->setContextProperty("modelCameraList", QVariant::fromValue(cameraNames));
    emit evCameraListChanged(cameraNames.size());

    // Avvio automatico USB solo se sink pronto e preview non ancora attivo
    if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF && m_pPreviewQVideoSink) {
        startUsbPreview(m_pPreviewQVideoSink);
        m_indexPreviewCamera = 0;
        emit previewCameraIndexChanged(0);
    }

    // Reset se lista vuota
    if (m_lCameraVideo.isEmpty()) {
        if (CAMERA_PREVIEW_UNDEF != m_indexPreviewCamera)
            setPreviewCameraIndex(CAMERA_PREVIEW_UNDEF);
    }

    /*
    // Sostituisci la lista
    if (m_lCameraVideo != newList) {
        m_lCameraVideo = newList;
        bListCameraChanged = true;
    }

    // --- Aggiorna ComboBox ---
    if (bListCameraChanged) {
        QStringList cameraNames;
        cameraNames.append(tr("Caméra 1 (USB)"));
        if (ipOnline)
            cameraNames.append(tr("Caméra 2 (IP)"));

        CoreApp::getInstanceCoreUiQml()
            ->getApplicationEngine()
            ->rootContext()
            ->setContextProperty("modelCameraList", QVariant::fromValue(cameraNames));

        emit evCameraListChanged(m_lCameraVideo.size());
    }

    // Avvio automatico — indice 0 sempre, camera reale o meno
    if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF) {
        setPreviewCameraIndex(0);
    }


    // Se il preview non è ancora attivo e c'è almeno una camera USB, avviala
    if (m_indexPreviewCamera == CAMERA_PREVIEW_UNDEF && !newList.isEmpty()) {
        // Cerca la prima camera USB
        for (auto cam : qAsConst(newList)) {
            if (!cam->isIpCamera()) {
                QTimer::singleShot(100, this, [this, cam]() {
                    setPreviewCameraIndex(cam->getCameraIndex());
                });
                break;
            }
        }
    }

    // --- Reset preview se lista vuota ---
    if (m_lCameraVideo.isEmpty()) {
        if (CAMERA_PREVIEW_UNDEF != m_indexPreviewCamera)
            setPreviewCameraIndex(CAMERA_PREVIEW_UNDEF);
    }
*/
}

void CameraManager::onUpdateCameraDatabase()
{
    if (!m_pVectorStd) return;

    // Auto-assegnazione dalla lista video
    for (auto c : qAsConst(m_lCameraVideo)) {
        for (auto db : qAsConst(m_lCameraDatabase)) {
            if (db->getCameraId() == c->getCameraId() &&
                db->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid) {
                if (!m_aCameraRole[db->getCameraRole()]->getIsReady()) {
                    m_aCameraRole[db->getCameraRole()]->setCamera(c);
                }
            }
        }
    }

    // Aggiorna il database per ogni ruolo pronto
    for (int r = 0; r < CameraRole::ROLE_MAX; r++) {
        if (!m_aCameraRole[r]->getIsReady()) continue;

        CameraVideo* pCamVideo = m_aCameraRole[r]->getCameraVideo();
        if (pCamVideo == nullptr) continue;  // GUARD fondamentale

        CameraDatabase* pCamDb = nullptr;
        for (auto i : qAsConst(m_lCameraDatabase)) {
            if (i->getProductUid() == m_pVectorStd->getFuncGeneralParam().m_paramDProdUid &&
                i->getCameraRole() == r) {
                pCamDb = i;
                break;
            }
        }

        if (nullptr == pCamDb) {
            // Nuova voce nel database — usa la camera del ruolo, MAI m_indexPreviewCamera
            pCamDb = new CameraDatabase(
                pCamVideo->getCameraId(),
                m_pVectorStd->getFuncGeneralParam().m_paramDProdUid,
                r);
            m_lCameraDatabase.append(pCamDb);
        } else {
            // Aggiorna l'ID esistente
            pCamDb->setCameraId(pCamVideo->getCameraId());
        }
    }
}
///////////////////////////////////////////////
/*
void CameraManager::onUpdateCameraDatabase()
{
    if (!m_pVectorStd) return;

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
            if (m_aCameraRole[r]->getCameraVideo() == nullptr) continue;
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
                if (m_indexPreviewCamera < 0 ||
                    m_indexPreviewCamera >= m_lCameraVideo.size()) {
                    continue; // salta se indice non valido
                }
                pCamDb = new CameraDatabase(m_lCameraVideo.at(m_indexPreviewCamera)->getCameraId(), m_pVectorStd->getFuncGeneralParam().m_paramDProdUid, r);
                m_lCameraDatabase.append(pCamDb);
            } else {
                // update product camera
                if (nullptr == pCamDb) {
                    // Usa il camera video del ruolo corrente, non l'indice preview
                    if (m_aCameraRole[r]->getCameraVideo() == nullptr) continue;
                    pCamDb = new CameraDatabase(
                        m_aCameraRole[r]->getCameraVideo()->getCameraId(),
                        m_pVectorStd->getFuncGeneralParam().m_paramDProdUid,
                        r);
                    m_lCameraDatabase.append(pCamDb);
                }
            }
        }
    }
}
*/
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

bool CameraManager::isMainCameraIp()
{
    int idx = getMainCameraIndex();
    if (idx == CameraVideo::CAMERA_INDEX_UNDEF) return false;
    // Cerca nella lista per indice fisico
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == idx) {
            return cam->isIpCamera();
        }
    }
    return false;
}

bool CameraManager::isPreviewCameraIp()
{
    // Indice 0 è sempre USB per definizione
    if (m_indexPreviewCamera == 0) return false;
    if (CAMERA_PREVIEW_UNDEF == m_indexPreviewCamera) return false;

    // Indice 1+ cerca nella lista
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == m_indexPreviewCamera) {
            return cam->isIpCamera();
        }
    }
    return false;
}

/*
bool CameraManager::isPreviewCameraIp()
{
    if (CAMERA_PREVIEW_UNDEF == m_indexPreviewCamera) return false;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == m_indexPreviewCamera) {
            return cam->isIpCamera();
        }
    }
    return false;
}
*/

/*
void CameraManager::openMainIpMedia()
{
    int i32MainIndex = getMainCameraIndex();
    if(CameraVideo::CAMERA_INDEX_UNDEF == i32MainIndex) {
        return;
    }
    CameraVideo* pFound = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == i32MainIndex) {
            pFound = cam;
            break;
        }
    }
    if (!pFound || !pFound->isIpCamera()) return;
    if (m_pMainPlayer) m_pMainPlayer->stop();
    QThread::msleep(200);   //Forces the current thread to sleep for msecs milliseconds. (TD)

    m_pMainSink->initCallbacks();

    //IpCamera* pIpCam = (IpCamera*) m_lCameraVideo.at(getMainCameraIndex());
    IpCamera* pIpCam = static_cast<IpCamera*>(pFound);
    pIpCam->openMedia();
    m_pMainPlayer->open(pIpCam->getMedia());
}
*/

void CameraManager::openMainIpMedia()
{
    // Cerca la prima camera IP nella lista
    IpCamera* pIpCam = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->isIpCamera()) {
            pIpCam = static_cast<IpCamera*>(cam);
            break;
        }
    }
    if (!pIpCam) return;

    if (m_pMainPlayer) m_pMainPlayer->stop();
    QThread::msleep(200);
    m_pMainSink->initCallbacks();
    pIpCam->openMedia();
    m_pMainPlayer->open(pIpCam->getMedia());
}

void CameraManager::openPreviewIpMedia()
{
    // Cerca la prima camera IP nella lista, indipendentemente dall'indice
    IpCamera* pIpCam = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->isIpCamera()) {
            pIpCam = static_cast<IpCamera*>(cam);
            break;
        }
    }
    if (!pIpCam) return;

    if (m_pPreviewPlayer) m_pPreviewPlayer->stop();
    m_pPreviewSink->initCallbacks();
    pIpCam->openMedia();
    m_pPreviewPlayer->open(pIpCam->getMedia());
}

/*
void CameraManager::openPreviewIpMedia()
{

    if(CAMERA_PREVIEW_UNDEF == m_indexPreviewCamera || m_indexPreviewCamera >= m_lCameraVideo.length()) {
        return;
    }

    CameraVideo* pFound = nullptr;
    for (auto cam : qAsConst(m_lCameraVideo)) {
        if (cam->getCameraIndex() == m_indexPreviewCamera) {
            pFound = cam;
            break;
        }
    }
    if (!pFound || !pFound->isIpCamera()) return;

    if (m_pPreviewPlayer) m_pPreviewPlayer->stop();

    m_pPreviewSink->initCallbacks();

    //IpCamera* pIpCam = (IpCamera*) m_lCameraVideo.at(m_indexPreviewCamera);
    IpCamera* pIpCam = static_cast<IpCamera*>(pFound);
    pIpCam->openMedia();
    m_pPreviewPlayer->open(pIpCam->getMedia());
}
*/

void CameraManager::connectMainIpSink(QObject* videoOutput)
{
    if (!videoOutput) return;
    QVideoSink* sink = videoOutput->property("videoSink").value<QVideoSink*>();
    if (!sink || !m_pMainSink) return;
    if (sink) {
        m_pMainSink->setVideoSink(sink);
        fLogInfo("connectMainIpSink: sink connected");
    }
}
