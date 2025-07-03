/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Loggable.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

// -- Qt includes
#include <QString>

// -- Internal includes
#include <log/Logger.h>

/**
 * @brief The Loggable class
 *
 * Inherit from this class to log message from your class.
 *
 * Exemple of output :
 * 29.05.2020 15:23:47.846 [X] - MaClasse::uneFonction : Mon message
 * where 'X' is the level (Fatal, Error, Warning, Info, Debug1, Debug2)
 */
class Loggable
{
public:
    Loggable(const QString & p_logName);
    virtual ~Loggable();

    // -- Convenient methods to log a message
    void logFatal(const QString & p_message) const;
    void logError(const QString & p_message) const;
    void logWarning(const QString & p_message) const;
    void logInfo(const QString & p_message) const;
    void logDebug1(const QString & p_message) const;
    void logDebug2(const QString & p_message) const;

    // -- Accessors
    void setLogName(const QString & p_logName);
    QString getLogName() const;

protected :
    QString m_logName;
};

// -- Macro
// The following marcros add the function name from where the log has been emitted
// These macros work only in a function of a Loggable class
// Exemple of output :
// 29.05.2020 15:23:47.846 [X] - MaClasse::uneFonction : Mon message
#define fLogFatal(MESSAGE)  Logger::logFatal(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
#define fLogError(MESSAGE)  Logger::logError(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
#define fLogWarning(MESSAGE)  Logger::logWarning(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
#define fLogInfo(MESSAGE)  Logger::logInfo(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
#define fLogDebug1(MESSAGE)  Logger::logDebug1(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
#define fLogDebug2(MESSAGE)  Logger::logDebug2(QString("%1::%2 : %3").arg(m_logName).arg(__FUNCTION__).arg(MESSAGE))
