/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Report.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/type/Report.h>


Report::Report(const QString & p_className) : ProjectDate(p_className)
{
    m_status = REPORT_UNKNOW;
    m_valueMin = 0;
    m_valueMax = 0;
}

Report::Report(const Report & p_value) : ProjectDate(p_value)
{

}

Report::~Report()
{

}

// -- Params

quint32 Report::getStatus(void) const
{
    return m_status;
}

void Report::setStatus(quint32 p_value)
{
    m_status = p_value;
}


qreal Report::getValueMin(void) const
{
    return m_valueMin;
}

void Report::setValueMin(qreal p_value)
{
    m_valueMin = p_value;
}

qreal Report::getValueMax(void) const
{
    return m_valueMax;
}

void Report::setValueMax(qreal p_value)
{
    m_valueMax = p_value;
}


// -- XmlProject

bool Report::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = ProjectDate::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(quint32, m_status);
    bRet &= XML_READ_UNIQUE(qreal, m_valueMin);
    bRet &= XML_READ_UNIQUE(qreal, m_valueMax);
    return bRet;
}

bool Report::customLoadDefaultParam()
{
    bool bRet = ProjectDate::customLoadDefaultParam();
    m_status = REPORT_UNKNOW;
    m_valueMin = 0;
    m_valueMax = 0;
    return bRet;
}

bool Report::customApplyParam()
{
    bool bRet = ProjectDate::customApplyParam();

    return bRet;
}

bool Report::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = ProjectDate::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_status);
    bRet &= XML_WRITE_UNIQUE(m_valueMin);
    bRet &= XML_WRITE_UNIQUE(m_valueMax);
    return bRet;
}
