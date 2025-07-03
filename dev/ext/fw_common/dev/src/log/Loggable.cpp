/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Loggable.cpp
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

// -- Internal includes
#include <log/Loggable.h>

Loggable::Loggable(const QString & p_logName)
{
    m_logName = p_logName;
}

Loggable::~Loggable()
{

}

// -- Convenient methods to log a message
void Loggable::logFatal(const QString & p_message) const
{
    Logger::logFatal(QString("%1 : %2").arg(m_logName).arg(p_message));
}

void Loggable::logError(const QString & p_message) const
{
    Logger::logError(QString("%1 : %2").arg(m_logName).arg(p_message));
}

void Loggable::logWarning(const QString & p_message) const
{
    Logger::logWarning(QString("%1 : %2").arg(m_logName).arg(p_message));
}

void Loggable::logInfo(const QString & p_message) const
{
    Logger::logInfo(QString("%1 : %2").arg(m_logName).arg(p_message));
}

void Loggable::logDebug1(const QString & p_message) const
{
    Logger::logDebug1(QString("%1 : %2").arg(m_logName).arg(p_message));
}

void Loggable::logDebug2(const QString & p_message) const
{
    Logger::logDebug2(QString("%1 : %2").arg(m_logName).arg(p_message));
}


// -- Accessors
void Loggable::setLogName(const QString & p_logName)
{
    m_logName = p_logName;
}

QString Loggable::getLogName() const
{
    return m_logName;
}


