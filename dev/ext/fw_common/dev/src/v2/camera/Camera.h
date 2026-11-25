/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Camera.h
# Author      : Thierry Lechevallier
# Date        : 09/11/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include "log/Loggable.h"
#include "xml/XmlSettings.h"
#include "xml/XmlProject.h"
// -- Qt includes
#include <QObject>

// ----------------------------------------------------------------------------
// -- CameraVideo

/**
 * @brief The CameraVideo class
 *
 * Qt camera object
 */

class CameraVideo : public QObject, public Loggable
{
    Q_OBJECT
public:
    CameraVideo(qint32 p_index, const QString& p_cameraId);
    ~CameraVideo();

    // -- Parameter
    Q_INVOKABLE QString getCameraId() const;

    // -- Behaviour
    Q_INVOKABLE qint32 getCameraIndex() const;
    Q_INVOKABLE void setCameraIndex(qint32 p_index);

    Q_INVOKABLE bool isIpCamera() const;

    static const qint32 CAMERA_INDEX_UNDEF = -1;

protected :

    // -- Behaviour
    qint32 m_index;                     ///< index in QCameraInfo::availableCameras();
    QString m_cameraId;                 ///< Value given by QCameraInfo.deviceId()
    bool m_bIsIpCamera = false;
};

// ----------------------------------------------------------------------------
// -- CameraRole

/**
 * @brief The CameraRole class
 *
 */

class CameraRole : public QObject, public Loggable
{
    Q_OBJECT
    Q_PROPERTY(bool isEnabled READ getIsEnabled NOTIFY evIsEnabledChanged)
    Q_PROPERTY(qint32 cameraIndex READ getCameraIndex NOTIFY evCameraIndexChanged)
    Q_PROPERTY(CameraVideo* cameraVideo READ getCameraVideo NOTIFY evCameraVideoChanged)
    Q_PROPERTY(bool isReady READ getIsReady NOTIFY evIsReadyChanged)

public:
    CameraRole(qint32 p_role);
    ~CameraRole();

    // -- Parameter
    qint32 getCameraIndex() const;
    CameraVideo *getCameraVideo() const;
    void setCamera(CameraVideo* p_camera);

    void setIsEnabled(bool p_value);
    bool getIsEnabled() const;

    void setIsReady(bool p_value);
    bool getIsReady() const;

    Q_INVOKABLE qint32 getRole() const;

    enum ROLE_CAMERA {
        ROLE_ALARM,
        ROLE_SENSOR1,
        ROLE_SENSOR2,
        ROLE_SENSOR3,
        ROLE_SENSOR4,
        ROLE_FRONT,
        ROLE_REAR,
        ROLE_MAX
    };

signals :
    void evIsEnabledChanged(bool);
    void evCameraIndexChanged(qint32);
    void evCameraVideoChanged(CameraVideo *);
    void evIsReadyChanged(bool);
    void evCameraIsReadyChanged(CameraRole*,bool);

private :
    void updateReadyStatus();

    // -- Parameter
    qint32 m_role;  ///< value in ROLE_CAMERA
    bool m_isEnabled;
    bool m_isReady;
    CameraVideo* m_pCamera;
};

// ----------------------------------------------------------------------------
// -- CameraDatabase

/**
 * @brief The CameraDatabase class
 *
 */

class CameraDatabase : public QObject, public Loggable, public XmlProject
{
    Q_OBJECT

public:
    CameraDatabase();
    CameraDatabase(const QString& p_cameraId, const QString& p_productUid, const qint32 p_role);
    ~CameraDatabase();

    // -- Parameter
    QString getCameraId() const;
    void setCameraId(const QString& p_value);

    QString getProductUid() const;
    void setProductUid(const QString& p_value);

    qint32 getCameraRole() const;
    void setCameraRole(qint32 p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    // -- Parameter
    QString m_cameraId;     ///< Id of the USB device
    QString m_productId;    ///< Id of the product associated to this camera
    qint32 m_role;          ///< Role of this camera for the given product
};
