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
#include <camera/Camera.h>
class VectorStd;

// -- Qt includes
#include <QObject>
#include <QList>
class QTimer;

/**
 * @brief The CameraManager class
 */

class CameraManager : public QObject, public XmlSettingsLoggable
{
    Q_OBJECT
    Q_PROPERTY(QCamera* previewCamera READ getPreviewCamera NOTIFY previewCameraChanged)
    Q_PROPERTY(QCamera* mainCamera READ getMainCamera NOTIFY mainCameraChanged)
    Q_PROPERTY(bool selectMainCameraAuto READ getMainCameraSelectAuto NOTIFY selectMainCameraAutoChanged)

public:
    static CameraManager * makeInstance();
    static CameraManager * getInstance();

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
    Q_INVOKABLE QCamera* getPreviewCamera() const;
    Q_INVOKABLE QCamera* getMainCamera() const;
    Q_INVOKABLE void assignPreviewAs(qint32 p_role) const;
    bool getMainCameraSelectAuto() const;
    void setMainCameraSelectAuto(bool p_value);

    Q_INVOKABLE void setProductRoleMainCamera(qint32 p_productRole);
    Q_INVOKABLE void setRoleMainCamera(qint32 p_role);
    Q_INVOKABLE void setPreviewCameraIndex(qint32 p_index);

    static const qint32 CAMERA_PREVIEW_UNDEF = -1;

signals :
    // -- Properties
    void previewCameraChanged(QCamera*);
    void previewCameraIndexChanged(qint32);
    void mainCameraChanged(QCamera*);
    void selectMainCameraAutoChanged(bool);

    // --
    void evCameraListChanged(qint32 p_value);   ///< number of items in the list


private slots :
    void onVectorProductIdChanged(qint32);
    void onVectorDeviceListChanged(qint32);
    void onUpdateCameraList();

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private:
    CameraManager();
    static CameraManager* m_pMyInstance;

    // -- Behaviour
    QList<CameraVideo* > m_lCameraVideo;            ///< list of available cameras, updated by timer
    CameraRole* m_aCameraRole[CameraRole::ROLE_MAX]; ///< list of camera by role
    QList<CameraDatabase* > m_lCameraDatabase;

    QTimer* m_pTimer;

    qint32 m_indexPreviewCamera;
    qint32 m_roleMainCamera;
    bool m_selectMainCameraAuto;          ///< function to the product, user can select le main camera or not

    // -- Managers
    VectorStd* m_pVectorStd;
};
