#pragma once

#include <v2/prot/ProtMsg.h>
#include <QByteArray>
#include <QList>

namespace ProtD4 {

static const char DEFAULT_DEVICE_IP[] = "192.168.10.20";

bool verifyAuthAnswer(const qint8 *p_request, const quint8 *p_answer);
QList<quint32> decodeFunctionList(const quint8 *p_payload, int p_len, bool *p_ok);
bool toWire(ProtMsg *p_msg);
bool fromWire(ProtMsg *p_msg);

inline qint32 decodeSample(const quint8 *p_bytes)
{
    const quint16 w = quint16(p_bytes[0]) | (quint16(p_bytes[1]) << 8);
    const quint16 r = w & 0x3FFF;
    return (r & 0x2000) ? qint32(r) - 0x4000 : qint32(r);
}

}