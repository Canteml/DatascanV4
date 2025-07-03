/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : StatisticManager.h
# Author      : Thierry Lechevallier
# Date        : 22/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <log/StatisticManager.h>

// -- Qt includes
#include <QTimer>
#include <QDateTime>
#include <QMutex>


// ----------------------------------------------------------------------------
// -- Class StatCounter

StatCounter::StatCounter(const QString &p_name) : XmlProject("StatCounter"),
    m_name(p_name)
{
    m_dataResettable = 0;
    m_dataTotal = 0;
}

StatCounter::StatCounter() : XmlProject("StatCounter")
{
    m_name = "";
    m_dataResettable = 0;
    m_dataTotal = 0;
}

StatCounter::~StatCounter()
{

}

QString StatCounter::getValueResettableString()
{
    return QString::number(m_dataResettable);
}

QString StatCounter::getValueTotalString()
{
    return QString::number(m_dataTotal);
}

quint64 StatCounter::getValueResettable()
{
    return m_dataResettable;
}

quint64 StatCounter::getValueTotal()
{
    return m_dataTotal;
}

QString StatCounter::getName() const
{
    return m_name;
}

void StatCounter::resetStatistic()
{
    m_dataResettable = 0;
    emit evValueChanged();
}

// -- XmlSettings
bool StatCounter::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;    
    bRet &= XML_READ_UNIQUE(QString, m_name);
    bRet &= XML_READ_UNIQUE(quint64, m_dataResettable);
    bRet &= XML_READ_UNIQUE(quint64, m_dataTotal);
    return bRet;
}

bool StatCounter::customLoadDefaultParam()
{
    m_dataResettable = 0;
    m_dataTotal = 0;
    return true;
}

bool StatCounter::customApplyParam()
{
    return true;
}

bool StatCounter::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    quint32 m_type = getType();
    bRet &= XML_WRITE_UNIQUE(m_type);
    bRet &= XML_WRITE_UNIQUE(m_name);
    bRet &= XML_WRITE_UNIQUE(m_dataResettable);
    bRet &= XML_WRITE_UNIQUE(m_dataTotal);
    return bRet;
}


// ----------------------------------------------------------------------------
// -- Class StatTime

StatEvent::StatEvent(const QString &p_name, const QString p_unit) : StatCounter(p_name), m_unit(p_unit)
{

}

StatEvent::StatEvent()
{
    m_name = "";
    m_unit = "";
}
StatEvent::~StatEvent()
{

}

QString StatEvent::getValueResettableString()
{
    QString sRet = QString::number(m_dataResettable);
    if( !m_unit.isEmpty() ) {
        sRet += " " + m_unit;
    }
    return sRet;
}

QString StatEvent::getValueTotalString()
{
    QString sRet = QString::number(m_dataTotal);
    if( !m_unit.isEmpty() ) {
        sRet += " " + m_unit;
    }
    return sRet;
}

quint32 StatEvent::getType(void) const
{
    return STAT_EVENT;
}

void StatEvent::eventOccur(quint32 p_count) {
    m_dataTotal += p_count;
    m_dataResettable += p_count;
    emit evValueChanged();
}

bool StatEvent::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = StatCounter::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(QString, m_unit);
    return bRet;
}

bool StatEvent::customLoadDefaultParam()
{
    bool bRet = StatCounter::customLoadDefaultParam();
    m_unit = "";
    return bRet;
}

bool StatEvent::customApplyParam()
{
    return StatCounter::customApplyParam();
}

bool StatEvent::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = StatCounter::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_unit);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- Class StatTime

StatTime::StatTime(const QString &p_name) : StatCounter(p_name)
{
    m_timerEnabled = false;
}

StatTime::StatTime()
{
    m_timerEnabled = false;
}

StatTime::~StatTime()
{
    m_timerEnabled = false;
}

QString StatTime::getValueResettableString()
{
    int h = m_dataResettable/3600;
    int m = (m_dataResettable - h*3600)/60;
    int s = (m_dataResettable - h*3600 - m*60);
    return QString("%1:%2:%3").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));
}

QString StatTime::getValueTotalString()
{
    int h = m_dataTotal/3600;
    int m = (m_dataTotal - h*3600)/60;
    int s = (m_dataTotal - h*3600 - m*60);
    return QString("%1:%2:%3").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));
}

quint32 StatTime::getType(void) const
{
    return STAT_TIME;
}


void StatTime::setTimerEnabled(bool p_enabled)
{
    m_timerEnabled = p_enabled;
}

void StatTime::enableTimer()
{
    m_timerEnabled = true;
}

void StatTime::disableTimer()
{
    m_timerEnabled = false;
}

bool StatTime::getTimerEnabled(void) const
{
    return m_timerEnabled;
}

void StatTime::incrementTimer()
{
    if( m_timerEnabled ) {
        m_dataResettable++;
        m_dataTotal++;
        emit evValueChanged();
    }
}

// ----------------------------------------------------------------------------
// -- Class StatisticManager


// -- Singleton
StatisticManager::StatisticManager() : Loggable("StatisticManager"), XmlSettings("StatisticManager")
{
    m_mStats = new QMutex(QMutex::Recursive);

    m_timUpdateStatTime = new QTimer();
    m_timUpdateStatTime->setSingleShot(false);
    connect(m_timUpdateStatTime, &QTimer::timeout, this, &StatisticManager::onUpdateStatTime);

    m_timEmitStatChanged = new QTimer();
    m_timEmitStatChanged->setSingleShot(true);
    connect(m_timEmitStatChanged, &QTimer::timeout, this, &StatisticManager::onStatChanged);
    m_epocLastEmitStatChanged = 0;
}

StatisticManager::~StatisticManager()
{
    deleteStats();
    delete m_mStats;
    m_mStats = nullptr;
}

// -- state chart
bool StatisticManager::initManager()
{
    return true;
}

bool StatisticManager::startManager()
{
    m_timUpdateStatTime->start(1000);
    return true;
}

void StatisticManager::stopManager()
{
    if( m_timUpdateStatTime->isActive() ) {
        m_timUpdateStatTime->stop();
    }
    if( m_timEmitStatChanged->isActive() ) {
        m_timEmitStatChanged->stop();
    }
}


// -- Data
const QList<StatCounter *> StatisticManager::getStats(void) const
{
    return m_lStats;
}

void StatisticManager::resetStatistics()
{
    m_lastReset = QDateTime::currentDateTimeUtc();
    m_mStats->lock();
    for(auto i : qAsConst(m_lStats)) {
        i->resetStatistic();
    }
    m_mStats->unlock();
}

QString StatisticManager::getLastResetDateString() const
{
    return m_lastReset.toString("hh:mm:ss dd.MM.yyyy");
}

quint32 StatisticManager::getDaysSinceLastResetDate() const
{
    QDateTime today = QDateTime::currentDateTime();
    qint64 deltaMs = today.msecsTo(m_lastReset);
    qint64 nbDays = abs(deltaMs / (24*60*60*1000));
    return (quint32)nbDays;
}


// -- SLOTS

void StatisticManager::onStatChanged(void)
{
    // emit only if manager is runing (given by m_timUpdateStatTime state)
    if( m_timUpdateStatTime->isActive() ) {
        // emit signal or wait ?
        qint64 curTime = QDateTime::currentMSecsSinceEpoch();
        qint64 dT1000 =  curTime - (m_epocLastEmitStatChanged + 1000);
        if( dT1000 > 0 ) {
            m_epocLastEmitStatChanged = curTime;
            emit evStatChanged();
        } else {
            // start timer to be called back later
            dT1000 = qMin(qAbs(dT1000), (qint64)1000);
            if( !m_timEmitStatChanged->isActive() ) {
                m_timEmitStatChanged->start(dT1000);
            }
        }
    }
}

void StatisticManager::onUpdateStatTime(void)
{
    // increment all StatTime
    for(auto i : qAsConst(m_lStatTime)) {
        i->incrementTimer();
    }
}



// -- Behaviour
void StatisticManager::addStat(StatCounter * p_pStat)
{
    m_mStats->lock();
    if( p_pStat->getType() == StatCounter::STAT_TIME ) {
        m_lStatTime.append(static_cast<StatTime*>(p_pStat));
    }
    m_lStats.append(p_pStat);
    m_mStats->unlock();

    connect(p_pStat, &StatCounter::evValueChanged, this, &StatisticManager::onStatChanged);
}

void StatisticManager::deleteStats()
{
    m_lStatTime.clear();
    for(auto i : qAsConst(m_lStats)) {
        delete i;
    }
    m_lStats.clear();
}


// -- XmlSettings
bool StatisticManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;

    bRet &= XML_READ_UNIQUE(QDateTime, m_lastReset);

    m_mStats->lock();
    deleteStats();
    makeStatistics();

    // Load stats : parse items
    QDomNode s = p_pParent->firstChild();
    if( s.isNull() ) {
        bRet = false;
    } else {
        int iL = 0;
        while(!s.isNull()) {
            QDomElement se = s.toElement();
            if( se.nodeName() == "StatCount" ) {
                // New Stat ! => witch type ?
//                StatCounter * sc = nullptr;
//                int seType = se.firstChildElement("m_type").text().toInt();
//                if( seType == StatCounter::STAT_EVENT ) {
//                    sc = new StatEvent();
//                    bRet &= sc->customLoadParam(&se);
//                } else if( seType == StatCounter::STAT_TIME ) {
//                    sc = new StatTime();
//                    bRet &= sc->customLoadParam(&se);
//                }

//                if( nullptr == sc ) {
//                    fLogError(QString("unknow type : %1").arg(seType));
//                    bRet = false;
//                } else {
                    if( iL < m_lStats.size() ) {
                        // add to the list
                        m_lStats.at(iL)->customLoadParam(&se);
                        iL++;
                    }
//                }
            }
            s = s.nextSiblingElement();

        }
    }

    m_mStats->unlock();
    return bRet;
}

bool StatisticManager::customLoadDefaultSettings()
{
    m_lastReset = QDateTime::currentDateTime();
    deleteStats();
    makeStatistics();
    return true;
}

bool StatisticManager::customApplySettings()
{
    return true;
}

bool StatisticManager::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;

    // clear all
    QDomNode nodes = p_pParent->firstChild();
    while(p_pParent->hasChildNodes())
    {
        p_pParent->removeChild(nodes);
        nodes = p_pParent->firstChild();
    }

    // set param
    bRet &= XML_WRITE_UNIQUE(m_lastReset);



    // -- statistics
    m_mStats->lock();
    for(auto i : qAsConst(m_lStats)) {
        QDomElement se = p_pDoc->createElement("StatCount");
        p_pParent->appendChild(se);
        bRet &= i->customSaveParam(p_pDoc, &se);
    }
    m_mStats->unlock();
    return bRet;
}
