/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyAScanAdapter.cpp
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#include <uskey/UsKeyAScanAdapter.h>
#include <uskey/UsKeyManager.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/scan/AScan.h>

// ----------------------------------------------------------------------------

UsKeyAScanAdapter::UsKeyAScanAdapter(QObject* parent)
    : QObject(parent)
{
}

void UsKeyAScanAdapter::bind(UsKeyManager* manager, VectorFuncAScan* funcAScan)
{
    unbind();
    m_funcAScan = funcAScan;
    connect(manager, &UsKeyManager::evNewRawAScan,
            this,    &UsKeyAScanAdapter::onNewRawAScan,
            Qt::QueuedConnection);
}

void UsKeyAScanAdapter::unbind()
{
    m_funcAScan = nullptr;
    disconnect();
}

// ----------------------------------------------------------------------------

void UsKeyAScanAdapter::onNewRawAScan(QVector<unsigned short> samples, qint64 timestampMs)
{
    if (!m_funcAScan)
        return;

    const int count = qMin(samples.size(), static_cast<int>(AScanRawSample::ASCAN_SAMPLE_SIZE));

    quint8 buf[AScanRawSample::ASCAN_SAMPLE_SIZE] = {};
    for (int i = 0; i < count; ++i)
        buf[i] = static_cast<quint8>(samples[i] >> 4);  // 12-bit → 8-bit

    m_funcAScan->injectRawSamples(buf, timestampMs);
}
