#ifndef IPCAMERA_H
#define IPCAMERA_H

#include "Camera.h"

#include <QObject>
#include <QTimer>
#include <QTime>

#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>

#define RTSP_PORT 554
#define IPCAM_TIMEOUT 1000     //10000
#define IPCAM_REFRESH_MS 1000   //4000

class QTcpSocket;

class IpCamera : public CameraVideo
{

    Q_OBJECT

    Q_PROPERTY(bool online READ isOnline NOTIFY onlineChanged)

public:

    IpCamera(const QString& p_cameraId, QString p_sAddress, VlcInstance* p_pInstance);
    ~IpCamera();

    void startRefreshing();

    bool isOnline();

    QString ipAddress() const;

    QString rtspAddress() const;

    VlcMedia* getMedia() const;

public slots:

    void refreshStatus();

    void onSocketReadyRead();

    void openMedia();

signals:

    void onlineChanged(const bool online);

protected:

    QString m_sAddress;
    VlcInstance* m_pInstance;
    QTcpSocket* m_pSocket;
    QTimer* m_refreshTimer;
    QTime m_lastContact;
    VlcMedia* m_media = nullptr;

};

#endif // IPCAMERA_H
