#pragma once
#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMutex>
#include <QSize>
#include <QAtomicInt>
#include <QPointer>

class VlcMediaPlayer;

class VlcVideoSink : public QObject
{
    Q_OBJECT

public:
    explicit VlcVideoSink(VlcMediaPlayer* player, QObject* parent = nullptr);
    ~VlcVideoSink();

    void setVideoSink(QVideoSink* sink);
    QVideoSink* videoSink() const { return m_pSink; }

    void initCallbacks();

private:
    QAtomicInt m_destroying{0};
    static void* onLock(void* opaque, void** planes);
    static void  onUnlock(void* opaque, void* picture, void* const* planes);
    static void  onDisplay(void* opaque, void* picture);

    VlcMediaPlayer* m_pPlayer = nullptr;
    QPointer<QVideoSink> m_pSink;
    QMutex          m_mutex;
    QImage          m_frame;
    const int       m_width  = 1280;
    const int       m_height = 720;
};
