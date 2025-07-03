/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Location1d.cpp
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include "Location1d.h"
#include <v2/scan/Surface1d.h>
#include <v2/scan/Zone1d.h>
#include <v2/scan/AScan.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/exam/ExamManager.h>
#include <v2/scan/DisplaySurface.h>

// -- Qt Includes

Location1d::Location1d(const QString& p_name) : Loggable("Location "+p_name)
{
    m_isRecording = false;
    m_name = p_name;
    m_zoneAutoStartFound = false;
    m_isProcessingAScan = false;

    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        m_aSurface1d[i] = new Surface1d(this);
        m_aDisplaySurface1d[i] = new DisplaySurface(this);
    }

    m_pThread = new QThread();
    this->moveToThread(m_pThread);

    connect(this, &Location1d::evTreatAScan, this, &Location1d::onTreatAScan);
    m_pThread->start();
    m_pExamManager = ExamManager::getInstance();
    connect(m_pExamManager, &ExamManager::evMeasureDisplayChanged, this, &Location1d::onMeasureDisplayChanged);
}

Location1d::~Location1d()
{
    m_isRecording = false;
    clearScan();    // use m_mutTreatAScan
    m_mutTreatAScan.lock();
    m_pThread->quit();
    m_pThread->wait(500);
    if( m_pThread->isRunning() ) {
        m_pThread->terminate();
    }
    delete m_pThread;
    m_pThread = nullptr;

    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        delete m_aSurface1d[i];
        m_aSurface1d[i] = nullptr;
        delete m_aDisplaySurface1d[i];
        m_aDisplaySurface1d[i] = nullptr;
    }
    m_mutTreatAScan.unlock();
}

void Location1d::clearScan()
{
    disconnect(this, &Location1d::evTreatAScan, this, &Location1d::onTreatAScan);
    m_mutTreatAScan.lock();
    for(auto i : qAsConst(m_lZone1d)) {
        delete i;
    }
    m_lZone1d.clear();
    m_lZone1dAppendOrder.clear();

    m_mAScanPending.lock();
    for(auto i : qAsConst(m_lAScanPending)) {
        delete i;
    }
    m_lAScanPending.clear();
    m_mAScanPending.unlock();

    m_mAScanStored.lock();
    for(auto i : qAsConst(m_lAScanStored)) {
        delete i;
    }
    m_lAScanStored.clear();
    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        m_aSurface1d[i]->clearData();
        m_aDisplaySurface1d[i]->clearData();
    }
    m_mAScanStored.unlock();
    m_zoneAutoStartFound = false;
    m_isProcessingAScan = false;
    m_mutTreatAScan.unlock();
    emit evTreatAScan();
    emit evLocationZoneChanged(this);
    connect(this, &Location1d::evTreatAScan, this, &Location1d::onTreatAScan);
}

QString Location1d::getName() const
{
    return m_name;
}

void Location1d::setName(const QString &p_value)
{
    m_name = p_value;
}

bool Location1d::isRecording(void) const
{
    return m_isRecording;
}

void Location1d::setRecord(bool p_value)
{
    m_isRecording = p_value;
}

void Location1d::deleteStoredAScan(AScan* p_pAScan)
{
    m_mAScanStored.lock();
    m_lAScanStored.removeAll(p_pAScan);
    m_mAScanStored.unlock();
    delete p_pAScan;
}

void Location1d::pushAScan(AScan* p_pAScan)
{
    if( m_isRecording && m_pExamManager->isRecording() ) {
        m_mAScanPending.lock();
        m_lAScanPending.append(p_pAScan);
        m_mAScanPending.unlock();
        if( !m_isProcessingAScan ) {
            m_isProcessingAScan = true;
            emit evTreatAScan();
        }
    } else {
        delete p_pAScan;
    }
}

void Location1d::zoneAddToCurrentPosition()
{
    qreal pos = VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLBscanPosMmX();
    zoneAddToPosition(pos);
}

void Location1d::zoneAddToPosition(qreal p_posMm)
{
    bool needNewZone = true;
    if( !m_lZone1d.isEmpty() ) {
        // find the closest zone from the current position
        for(int i=0; i<m_lZone1d.size(); i++) {
            Zone1d* z = m_lZone1d.at(i);
            qreal deltaOr = abs(z->getOriginXMm() - p_posMm);

            if( deltaOr < DISTANCE_MIN_ZONE_MM && needNewZone ) {
                // update the zone close to current position, do not add new zone
                z->setOriginXMm(p_posMm);
                needNewZone = false;
                // this zone moved become the "lastest" zone added
                m_lZone1dAppendOrder.removeAll(z);
                m_lZone1dAppendOrder.append(z);
            }
        }
    }

    if( needNewZone ) {
        // add new zone
        Zone1d* pZone1d = new Zone1d();
        pZone1d->setOriginXMm(p_posMm);        
        // find index where add it.
        // The zones are sorted by growing Origin value.
        qint32 indexInsert = -1;
        if( m_lZone1d.isEmpty() ) {
            indexInsert = 0;
        } else {
            for(int i=0; i<m_lZone1d.size(); i++) {
                if( p_posMm < m_lZone1d.at(i)->getOriginXMm() ) {
                    indexInsert = i; // found where insert
                    break;
                }
            }
        }
        if( -1 == indexInsert ) {
            // not possible to insert (highest position value) => push back
            m_lZone1d.append(pZone1d);
        } else {
            m_lZone1d.insert(indexInsert, pZone1d);
        }
        m_lZone1dAppendOrder.append(pZone1d);   // append to the end (newest)
    }

    emit evLocationZoneChanged(this);
}

void Location1d::zoneFixBounds()
{
    // We consider the vector is positionned close to the end of the acquisition.
    // Find the start and the end position of the acquisition.
    qreal pos = VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLBscanPosMmX();
    m_aSurface1d[0]->lockData();
    qreal distFirst =  1000*m_aSurface1d[0]->getArrayPositionM()->first();
    qreal distLast =  1000*m_aSurface1d[0]->getArrayPositionM()->last();
    m_aSurface1d[0]->unlockData();
    qreal distFromBegin = qAbs(pos - distFirst);
    qreal distFromEnd = qAbs(pos - distLast);
    // origin is the further away point from the actual position
    qreal start = distFirst;
    qreal end = distLast;
    if( distFromEnd > distFromBegin ) {
        start = distLast;
        end = distFirst;
    }

    // is there a zone close to the start ?
    zoneAddToPosition(start);

    // Find the zone close to the end and set its size to match with the end of acquisition
    Zone1d* pZEnd = m_lZone1d.first();
    qreal closestDistFromEnd = qAbs(end - pZEnd->getOriginXMm());

    if( m_lZone1d.size() > 1 ) {
        for(int i=1; i<m_lZone1d.size(); i++) {
            Zone1d* z = m_lZone1d.at(i);
            qreal d = qAbs(end - z->getOriginXMm());
            if( d < closestDistFromEnd ) {
                pZEnd = z;
                closestDistFromEnd = d;
            }
        }
    }
    pZEnd->setSizeXMm(end-pZEnd->getOriginXMm());

    emit evLocationZoneChanged(this);
}

void Location1d::zoneRemoveLast()
{
    if( m_lZone1dAppendOrder.size() ) {
        Zone1d * z = m_lZone1dAppendOrder.takeLast();
        m_lZone1d.removeAll(z);
        delete z;
        emit evLocationZoneChanged(this);
    }
}

const QList<Zone1d*>* Location1d::getZones(void) const
{
    return &m_lZone1d;
}

Surface1d * Location1d::getSurface(qint32 p_index) const
{
    Surface1d * pRet = nullptr;
    if( 0<=p_index && p_index<VectorStd::NB_FUNC_ASCAN ) {
        pRet = m_aSurface1d[p_index];
    }
    return pRet;
}

DisplaySurface * Location1d::getDisplaySurface(qint32 p_index) const
{
    DisplaySurface * pRet = nullptr;
    if( 0<=p_index && p_index<VectorStd::NB_FUNC_ASCAN ) {
        pRet = m_aDisplaySurface1d[p_index];
    }
    return pRet;
}

void Location1d::onTreatAScan()
{
    bool bContinue = false;
    AScan* pAScan =  nullptr;
    m_mAScanPending.lock();
    bContinue = m_lAScanPending.size();
    if( bContinue ) {
        pAScan = m_lAScanPending.takeFirst();
    }
    m_mAScanPending.unlock();

    while(bContinue) {
        // treat ASCAN
        m_mutTreatAScan.lock();
        qint32 posIndex = m_aSurface1d[pAScan->m_source]->pushAScan(pAScan);
        m_aDisplaySurface1d[pAScan->m_source]->pushAScan(pAScan);
        // check auto zone
        if( m_pExamManager->getZoneAuto() ) {
            if( -1 != posIndex ) {
                qreal m;
                if( m_pExamManager->getMeasureDisplay() == ExamManager::MEASURE_1E ) {
                    m = m_aSurface1d[pAScan->m_source]->getArrayMeasure1e()->at(posIndex);
                } else {
                    m = m_aSurface1d[pAScan->m_source]->getArrayMeasureMe()->at(posIndex);
                }
                if( !m_zoneAutoStartFound ) {


                    if( m == 0 ) {
                        m_zoneAutoStartFound = true;
                        m_zoneAutoHoleEndPos.clear();
                        m_zoneAutoStartPosMm = m_aSurface1d[pAScan->m_source]->indexToPosition(posIndex);
                    }
                } else {
                    if( m != 0 ) {
                        qreal pos = m_aSurface1d[pAScan->m_source]->indexToPosition(posIndex);
                        if( !m_zoneAutoHoleEndPos.contains(pos) ) {
                            m_zoneAutoHoleEndPos.append(pos);
                            // filter : minimum 3 good measures to end a hole
                            if( m_zoneAutoHoleEndPos.size() > 3 ) {
                                m_zoneAutoStartFound = false;
                                qreal holeEndPos = m_zoneAutoHoleEndPos.first();
                                qreal deltaPos = abs(holeEndPos - m_zoneAutoStartPosMm);
                                if( deltaPos >= m_pExamManager->getZoneAutoHoleSizeMm() ) {
                                    zoneAddToPosition((m_zoneAutoStartPosMm + holeEndPos)/2);
                                }
                            }
                        }
                    } else {
                        m_zoneAutoHoleEndPos.clear();   // 0 => not contigus
                    }
                }
            }
        }

        // for now, only draw bscan for source0
        if( 0 == pAScan->m_source ) {
            emit evRequestSurfaceUpdate(m_aDisplaySurface1d[0]);
        }


        // store ASCAN
        if( -1 != posIndex ) {
            m_mAScanStored.lock();
            m_lAScanStored.append(pAScan);
            m_mAScanStored.unlock();
        } else {
            delete pAScan;
        }

        m_mutTreatAScan.unlock();

        // Continue ?
        m_mAScanPending.lock();
        bContinue = m_lAScanPending.size();
        if( bContinue ) {
            pAScan = m_lAScanPending.takeFirst();
        }
        m_mAScanPending.unlock();
    }

    m_isProcessingAScan = false;
}

void Location1d::onMeasureDisplayChanged()
{
    resetAutoZone();
}

void Location1d::resetAutoZone()
{
    m_zoneAutoStartFound = false;
    m_zoneAutoHoleEndPos.clear();
}
