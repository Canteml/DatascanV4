/*============================================================================
#
#   Copyright Roboplanet 2025
#
# ----------------------------------------------------------------------------
#
# File name   : UsKeyAScanAdapter.h
# Author      : Emanuele Luigi Cantarelli
# Date        : 16/06/2025
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QVector>

class UsKeyManager;
class VectorFuncAScan;

// ----------------------------------------------------------------------------
// -- UsKeyAScanAdapter
//
// Bridges UsKeyManager → VectorFuncAScan.
//
// Receives raw 16-bit A-scan data from UsKeyManager::evNewRawAScan,
// converts each sample from 12-bit (0–4095) to 8-bit (0–255) via >> 4,
// then calls VectorFuncAScan::injectRawSamples() to feed the existing pipeline.
//
// The position for each A-scan is read automatically from VectorFuncPosition
// inside injectRawSamples(), so encoder integration is transparent.

class UsKeyAScanAdapter : public QObject
{
    Q_OBJECT

public:
    explicit UsKeyAScanAdapter(QObject* parent = nullptr);

    void bind(UsKeyManager* manager, VectorFuncAScan* funcAScan);
    void unbind();

private slots:
    void onNewRawAScan(QVector<unsigned short> samples, qint64 timestampMs);

private:
    VectorFuncAScan* m_funcAScan = nullptr;
};
