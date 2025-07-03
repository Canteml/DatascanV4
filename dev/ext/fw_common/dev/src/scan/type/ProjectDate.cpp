/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ProjectDate.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/type/ProjectDate.h>


ProjectDate::ProjectDate(const QString & p_className) : XmlProjectLoggable(p_className)
{
    m_date = "";
}

ProjectDate::ProjectDate(const ProjectDate & p_value) : QObject(), XmlProjectLoggable(p_value.getXmlSectionName())
{
    (*this) = p_value;
}

ProjectDate::~ProjectDate()
{

}

QDateTime ProjectDate::getDateTime() const
{
    fLogWarning("TO DO !");
    return QDateTime();
}

QDate ProjectDate::getDate() const
{
    fLogWarning("TO DO !");
    return QDate();
}

QString ProjectDate::getDateString() const
{
    return m_date;
}

void ProjectDate::setDate(const QDateTime & p_value)
{
    m_date = p_value.toString("hh:mm:ss dd/MM/yyyy");
}

void ProjectDate::setDate(const QDate & p_value)
{
    m_date = p_value.toString("dd/MM/yyyy");
}

void ProjectDate::setDate(int p_hour, int p_minute, int p_second, int p_day, int p_month, int p_year)
{
    m_date = QString("%1:%2:%3 %4/%5/%6").arg(p_hour).arg(p_minute).arg(p_second).arg(p_day).arg(p_month).arg(p_year);
}

void ProjectDate::setDate(int p_day, int p_month, int p_year)
{
    m_date = QString("%1/%2/%3").arg(p_day).arg(p_month).arg(p_year);
}

void ProjectDate::setDateToNow(void)
{
    m_date = QDateTime::currentDateTime().toString("hh:mm:ss dd/MM/yyyy");
}

bool ProjectDate::isDateSet() const
{
    return !m_date.isEmpty();
}



// -- XmlProject
bool ProjectDate::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_date);
    return bRet;
}

bool ProjectDate::customLoadDefaultParam()
{
    m_date = "";
    return true;
}

bool ProjectDate::customApplyParam()
{
    return true;
}

bool ProjectDate::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_date);
    return bRet;
}


// -- operator
ProjectDate & ProjectDate::operator= (const ProjectDate & p_value)
{
    m_date = p_value.getDateString();
    return (*this);
}

bool ProjectDate::operator== (const ProjectDate & p_value)
{
    return m_date == p_value.getDateString();
}

