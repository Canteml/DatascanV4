#include "VlcVideoSink.h"
#include <VLCQtCore/MediaPlayer.h>
#include <vlc/vlc.h>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QDebug>

VlcVideoSink::VlcVideoSink(VlcMediaPlayer* player, QObject* parent)
    : QObject(parent), m_pPlayer(player)
{
}

void VlcVideoSink::initCallbacks()
{
    if (!m_pPlayer) return;
    libvlc_media_player_t* nativePlayer = m_pPlayer->core();
    if (!nativePlayer) return;

    libvlc_media_player_set_hwnd(nativePlayer, nullptr);
    libvlc_video_set_callbacks(nativePlayer, onLock, onUnlock, onDisplay, this);
    libvlc_video_set_format(nativePlayer, "RV32",
                            m_width, m_height, m_width * 4);
}

VlcVideoSink::~VlcVideoSink()
{

    m_destroying.storeRelease(1);
    m_pSink = nullptr;
}

void VlcVideoSink::setVideoSink(QVideoSink* sink)
{
    m_pSink = sink;
}

void* VlcVideoSink::onLock(void* opaque, void** planes)
{
    VlcVideoSink* self = static_cast<VlcVideoSink*>(opaque);
    self->m_mutex.lock();
    if (self->m_frame.isNull())
        self->m_frame = QImage(self->m_width, self->m_height, QImage::Format_RGB32);
    *planes = self->m_frame.bits();
    return nullptr;
}

void VlcVideoSink::onUnlock(void* opaque, void*, void* const*)
{
    static_cast<VlcVideoSink*>(opaque)->m_mutex.unlock();
}

void VlcVideoSink::onDisplay(void* opaque, void*)
{
    VlcVideoSink* self = static_cast<VlcVideoSink*>(opaque);
    if (self->m_destroying.loadAcquire()) return;

    QPointer<QVideoSink> sinkPtr = self->m_pSink;

    /////
    static int count = 0;
    if (count < 5) {
        qDebug() << "onDisplay called, sink:" << (sinkPtr ? "OK" : "NULL") << "count:" << count;
        count++;
    }

    if (!sinkPtr) return;

    QMutexLocker locker(&self->m_mutex);
    QVideoFrameFormat fmt(QSize(self->m_width, self->m_height),
                          QVideoFrameFormat::Format_BGRX8888);
    QVideoFrame frame(fmt);
    if (frame.map(QVideoFrame::WriteOnly)) {
        memcpy(frame.bits(0), self->m_frame.constBits(),
               self->m_frame.sizeInBytes());
        frame.unmap();
    }

    QMetaObject::invokeMethod(sinkPtr, [sinkPtr, frame]() {
        if (sinkPtr)
            sinkPtr->setVideoFrame(frame);
    }, Qt::QueuedConnection);
}

