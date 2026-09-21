#pragma once
#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>

class VlcQmlVideoSink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* videoSink READ videoSink WRITE setVideoSink)
public:
    explicit VlcQmlVideoSink(QObject* parent = nullptr);
    void setVideoSink(QObject* sink);      // solo dichiarazione, niente corpo
    QVideoSink* videoSink() const { return m_sink; }
    void presentFrame(const QVideoFrame& frame);

private:
    QVideoSink* m_sink = nullptr;
};
