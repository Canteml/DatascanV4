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
// -- Qt includes
#include <QObject>
#include <QCamera>
#include <QCameraImageCapture>


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
    CameraVideo(QCamera* p_camera, const QString& p_cameraId);
    ~CameraVideo();

    // -- Parameter
    Q_INVOKABLE QString getCameraId() const;

    // -- Behaviour
    Q_INVOKABLE QCamera* getCamera() const;

    QString capture(const QString& p_prefix);  ///< return filename (auto generated in project directory)


private :

    // -- Behaviour
    QCamera* m_pCamera;                 ///< Object in QCameraInfo::availableCameras();
    QCameraImageCapture* m_pCapture;    ///< Record pictures
    QString m_cameraId;                 ///< Value given by QCameraInfo.deviceId()
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
    Q_PROPERTY(QCamera* camera READ getCamera NOTIFY evCameraChanged)
    Q_PROPERTY(CameraVideo* cameraVideo READ getCameraVideo NOTIFY evCameraVideoChanged)
    Q_PROPERTY(bool isReady READ getIsReady NOTIFY evIsReadyChanged)

public:
    CameraRole(qint32 p_role);
    ~CameraRole();

    // -- Parameter
    QCamera* getCamera() const;
    CameraVideo *getCameraVideo() const;
    void setCamera(CameraVideo* p_camera);



    void setIsEnabled(bool p_value);
    bool getIsEnabled() const;

    void setIsReady(bool p_value);
    bool getIsReady() const;

    qint32 getRole() const;

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
    void evCameraChanged(QCamera *);
    void evCameraVideoChanged(CameraVideo *);
    void evIsReadyChanged(bool);

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

class CameraDatabase : public QObject, public Loggable, public XmlSettings
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

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    // -- Parameter
    QString m_cameraId;     ///< Id of the USB device
    QString m_productId;    ///< Id of the product associated to this camera
    qint32 m_role;          ///< Role of this camera for the given product
};
