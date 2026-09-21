#include "camera/VlcQmlVideoSink.h"

VlcQmlVideoSink::VlcQmlVideoSink(QObject* parent) : QObject(parent) {}

void VlcQmlVideoSink::setVideoSink(QObject* sink)
{
    m_sink = qobject_cast<QVideoSink*>(sink);
}

void VlcQmlVideoSink::presentFrame(const QVideoFrame& frame)
{
    if (m_sink)
        m_sink->setVideoFrame(frame);
}
