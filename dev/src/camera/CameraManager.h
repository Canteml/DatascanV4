/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : CameraManager.h
# Author      : Thierry Lechevallier
# Date        : 09/11/2021
#
# ==========================================================================*/


#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
#include <v2/camera/Camera.h>
#include <v2/camera/IpCamera.h>
class VectorStd;
class GlobalManager;
class ExamManager;

// -- Qt includes
#include <QObject>
#include <QList>
class QTimer;
class QCamera;

class VlcInstance;
class VlcMediaPlayer;
class VlcQmlSource;

/**
 * @brief The CameraManager class
 */

#define IP_CAMERA_MIN 54
#define IP_CAMERA_MAX 54

class CameraManager : public QObject, public XmlSettingsLoggable
{
    Q_OBJECT
    Q_PROPERTY(qint32 previewCameraIndex READ getPreviewCameraIndex NOTIFY previewCameraIndexChanged)
    Q_PROPERTY(qint32 mainCameraIndex READ getMainCameraIndex NOTIFY mainCameraIndexChanged)
    Q_PROPERTY(bool selectMainCameraAuto READ getMainCameraSelectAuto NOTIFY selectMainCameraAutoChanged)
    Q_PROPERTY(bool mainCameraIsAlarm READ getMainCameraIsAlarm NOTIFY mainCameraIsAlarmChanged)
    Q_PROPERTY(bool isPreviewCameraIp READ isPreviewCameraIp NOTIFY isPreviewCameraIpChanged)
    Q_PROPERTY(bool isMainCameraIp READ isMainCameraIp NOTIFY isMainCameraIpChanged)
    //Q_PROPERTY(VlcQmlSource* previewCameraSource READ getPreviewCameraSource WRITE setPreviewCameraSource NOTIFY previewCameraSourceChanged)


public:
    static CameraManager * makeInstance();
    static CameraManager * getInstance();
    void initQmlObject(QObject * p_pQmlTopLevel);

    void start();
    void stop();

    ~CameraManager();

    // -- Roles
    Q_INVOKABLE CameraRole* camAlarm();
    Q_INVOKABLE CameraRole* camSensor1();
    Q_INVOKABLE CameraRole* camSensor2();
    Q_INVOKABLE CameraRole* camSensor3();
    Q_INVOKABLE CameraRole* camSensor4();
    Q_INVOKABLE CameraRole* camFront();
    Q_INVOKABLE CameraRole* camRear();

    // -- Cameras
    Q_INVOKABLE qint32 getPreviewCameraIndex() const;
    Q_INVOKABLE qint32 getMainCameraIndex() const;
    Q_INVOKABLE QString getMainCameraId() const;
    bool getMainCameraIsAlarm() const;
    Q_INVOKABLE void assignPreviewAs(qint32 p_role);
    bool getMainCameraSelectAuto() const;
    void setMainCameraSelectAuto(bool p_value);

    Q_INVOKABLE void setProductRoleMainCamera(qint32 p_productRole);    ///< QML uses list of role function to the product
    Q_INVOKABLE void setProductRoleMainCameraNoAlarm(qint32 p_productRole);    ///< QML uses list of role function to the product
    Q_INVOKABLE qint32 getProductRoleMainCamera();                      ///< QML uses list of role function to the product
    Q_INVOKABLE void setRoleMainCamera(qint32 p_role);
    Q_INVOKABLE void setPreviewCameraIndex(qint32 p_index);

    static const qint32 CAMERA_PREVIEW_UNDEF = -1;

    Q_INVOKABLE QString captureMainCamera(const QString& p_prefix = "");
    Q_INVOKABLE void signalMainCameraCaptureSaved(QString p_fileName);

    Q_INVOKABLE VlcQmlSource* getPreviewCameraSource();
    Q_INVOKABLE VlcQmlSource* getMainCameraSource();
    Q_INVOKABLE bool isMainCameraIp();
    Q_INVOKABLE bool isPreviewCameraIp();
    Q_INVOKABLE void openMainIpMedia();
    Q_INVOKABLE void openPreviewIpMedia();

signals :
    // -- Properties
    void previewCameraIndexChanged(qint32 p_index);
    void mainCameraIndexChanged(qint32 p_index);
    void selectMainCameraAutoChanged(bool);
    void mainCameraIsAlarmChanged(bool);

    // --
    void evCameraListChanged(qint32 p_value);   ///< number of items in the list
    void evMainCameraReady();
    void evAlarmCameraReady();
    void evRequestMainCameraCapture(QString p_fileName);
    void evMainCameraCaptureSaved(QString);

    void isPreviewCameraIpChanged(bool);
    void isMainCameraIpChanged(bool);

private slots :
    void onVectorProductIdChanged(qint32);
    void onVectorDeviceListChanged(qint32);
    void onUpdateCameraList();
    void onUpdateCameraDatabase();
    void onGuiPageChanged(quint32);

    void onCameraIsReadyChanged(CameraRole*,bool);
    void onExamControlTypeChanged(quint32);
    void onMainCameraIndexChanged(quint32);
    void onRefreshCameraAutoReco();

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Properties
    void setMainCameraIsAlarm(bool p_value);

private:
    CameraManager();
    static CameraManager* m_pMyInstance;

    // -- Behaviour

    QList<CameraVideo* > m_lCameraVideo;             ///< list of available cameras, updated by timer
    QList<IpCamera*> m_lIpCamera;                    ///< list of available ip cameras, updated by timer

    VlcInstance* m_pInstance;
    VlcMediaPlayer* m_pMainPlayer;
    VlcMediaPlayer* m_pPreviewPlayer;
    VlcQmlSource* m_pMainSource;
    VlcQmlSource* m_pPreviewSource;

    CameraRole* m_aCameraRole[CameraRole::ROLE_MAX]; ///< list of camera by role
    QStringList m_slCameraRole;
    QList<CameraDatabase* > m_lCameraDatabase;

    QTimer* m_pTimer;
    QTimer* m_pTimerRefreshMainCam;

    qint32 m_indexPreviewCamera;
    qint32 m_roleMainCamera;
    bool m_selectMainCameraAuto;          ///< function to the product, user can select le main camera or not
    bool m_mainCameraIsAlarm;

    // -- Managers
    VectorStd* m_pVectorStd;
    GlobalManager* m_pGlobalManager;
    ExamManager* m_pExamManager;

    // -- QML components
    QCamera* m_pCameraPreview;
    QCamera* m_pCameraAlarm;
    QCamera* m_pCameraMain;

};
