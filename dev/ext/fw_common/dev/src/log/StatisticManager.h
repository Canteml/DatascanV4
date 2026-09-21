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

#pragma once


// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlSettings.h>
#include <xml/XmlProject.h>

// -- Qt includes
#include <QObject>
#include <QDateTime>
class QTimer;
class QMutex;

// ----------------------------------------------------------------------------
// -- Class StatCounter

/**
 * @brief The StatCounter class
 *
 * Generic class to make a statistic
 *
 */
class StatCounter : public QObject, public XmlProject
{
    Q_OBJECT
public :

    enum StatType {
        STAT_UNDEF,
        STAT_TIME,
        STAT_EVENT
    };

    StatCounter(const QString &p_name);
    StatCounter();
    virtual ~StatCounter();

    virtual QString getValueResettableString();
    virtual QString getValueTotalString();
    quint64 getValueResettable();
    quint64 getValueTotal();
    virtual quint32 getType(void) const = 0;
    QString getName() const;
    void resetStatistic(void);

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

signals :
    void evValueChanged();

protected :   

    quint64 m_dataResettable;
    quint64 m_dataTotal;
    QString m_name;
};


// ----------------------------------------------------------------------------
// -- Class StatTime

/**
 * @brief The StatTime class
 *
 */
class StatEvent : public StatCounter
{
    Q_OBJECT
public :

    StatEvent(const QString &p_name, const QString p_unit);
    StatEvent();
    virtual ~StatEvent();

    virtual QString getValueResettableString() override;
    virtual QString getValueTotalString() override;

    quint32 getType(void) const override;

    void eventOccur(quint32 p_count = 1);

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    QString m_unit; ///< to be added in settings
};

// ----------------------------------------------------------------------------
// -- Class StatTime

/**
 * @brief The StatTime class
 *
 */
class StatTime : public StatCounter
{
    Q_OBJECT
public :

    StatTime(const QString &p_name);
    StatTime();
    virtual ~StatTime();

    QString getValueResettableString() override;
    QString getValueTotalString() override;
    quint32 getType(void) const override;

    void setTimerEnabled(bool p_enabled);
    void enableTimer();
    void disableTimer();
    bool getTimerEnabled(void) const;

    void incrementTimer();

protected :
    bool m_timerEnabled;

};

// ----------------------------------------------------------------------------
// -- Class StatisticManager

/**
 * @brief The StatCounter class
 *
 * Manage all statistics
 *
 */

class StatisticManager : public QObject, public Loggable, public XmlSettings
{
    Q_OBJECT
public:
    // static Logger * makeInstance(); => derived class must implement their own instance maker
    // static Logger * getInstance(); => derived class must implement their own instance getter
    virtual ~StatisticManager();

    // -- state chart
    bool initManager();  ///< initialize the StatisticManager
    bool startManager(); ///< start the StatisticManager
    void stopManager();  ///< stop the StatisticManager

    // -- Data
    const QList<StatCounter *> getStats(void) const;

    // -- Reset
    void resetStatistics(void);
    QString getLastResetDateString() const;
    quint32 getDaysSinceLastResetDate() const;

signals :
    void evStatChanged();

protected slots :
    void onStatChanged(void);       ///< be aware that a statistic has changed
    void onUpdateStatTime(void);

protected :
    // -- Behaviour
    void addStat(StatCounter * p_pStat);    
    virtual void makeStatistics(void) = 0;   ///< derived classes must make here the statistics
    virtual void deleteStats();

    // -- XmlSettings
    bool customLoadSettings(QDomElement * p_pParent) override;
    bool customLoadDefaultSettings() override;
    bool customApplySettings() override;
    bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Singleton
    StatisticManager();

    // -- Behaviour
    QList<StatCounter *> m_lStats;
    QList<StatTime *> m_lStatTime;
    QTimer * m_timUpdateStatTime;
    QTimer * m_timEmitStatChanged;  ///< avoid emit too much signals
    qint64 m_epocLastEmitStatChanged;

    QRecursiveMutex* m_mStats;  ///< protect m_lStats
    QDateTime m_lastReset;
};
