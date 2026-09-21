#include "IpCamera.h"

#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QThread>

IpCamera::IpCamera(const QString& p_cameraId, QString p_sAddress, VlcInstance* p_pInstance)
    : CameraVideo(0, p_cameraId)
{
    m_bIsIpCamera = true;
    m_sAddress = p_sAddress;
    m_pInstance = p_pInstance;
    m_pSocket = new QTcpSocket();
    connect(m_pSocket, &QTcpSocket::readyRead, this, &IpCamera::onSocketReadyRead);
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(IPCAM_REFRESH_MS);
    m_lastContact = QTime::currentTime();
    m_lastContact = m_lastContact.addSecs(-10);
    connect(m_refreshTimer, &QTimer::timeout, this, &IpCamera::refreshStatus);
}

IpCamera::~IpCamera()
{
    m_refreshTimer->stop();
    delete m_refreshTimer;
    delete m_media;
    m_media = nullptr;
    m_refreshTimer = nullptr;
    m_pSocket->close();
    delete m_pSocket;
    m_pSocket = nullptr;
    m_pInstance = nullptr;
}

void IpCamera::startRefreshing()
{
    m_refreshTimer->start();
    refreshStatus();
}

bool IpCamera::isOnline()
{
    return m_lastContact.msecsTo(QTime::currentTime()) < IPCAM_TIMEOUT;
}

QString IpCamera::ipAddress() const
{
    return m_sAddress;
}

QString IpCamera::rtspAddress() const
{

    /*
    return QString("rtsp://") + QString("admin:R0b0planet@") + m_sAddress + ":" + QString::number(RTSP_PORT)
                + QString("/Streaming/Channels/102/?transportmode=multicast");

    */
    return QString("rtsp://") + QString("admin:R0b0planet@") + m_sAddress
           + QString("/onvif/live");

}

VlcMedia* IpCamera::getMedia() const
{
    return m_media;
}

void IpCamera::refreshStatus()
{
    m_pSocket->abort();
    m_pSocket->connectToHost(m_sAddress, RTSP_PORT);
    if(!m_pSocket->isOpen()) {

        return;
    }
    QByteArray message("OPTIONS ");
    message += rtspAddress().toUtf8() + QByteArray(" RTSP/1.0\r\n");
    message += "CSeq: 1\r\n";
    message += "\r\n";


    m_pSocket->write(message);
}

void IpCamera::onSocketReadyRead()
{
    QByteArray response = m_pSocket->readAll();
    if(response.startsWith("RTSP/1.0 200 OK\r\n")) {
        m_lastContact = QTime::currentTime();

    }

}

void IpCamera::openMedia()
{

    if(m_media != nullptr) {
        delete m_media;
    }
    m_media = new VlcMedia(rtspAddress(), m_pInstance);
}
