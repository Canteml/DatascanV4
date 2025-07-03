/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Zone.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/zone/Location.h>

Location::Location() : XmlProjectLoggable("Location")
{

}

Location::Location(const Location& p_value) : XmlProjectLoggable("Location")
{
    (*this) = p_value;
}

Location::~Location()
{

}


// -- XmlProject
bool Location::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(quint32, m_locationId);
    bRet &= XML_READ_UNIQUE(QString, m_locationName);
    bRet &= m_locationTransformation.customLoadParam(p_pParent);
    return bRet;
}

bool Location::customLoadDefaultParam()
{
    bool bRet = true;
    m_locationId = 0;
    m_locationName = "";
    m_locationTransformation.customLoadDefaultParam();
    return bRet;
}

bool Location::customApplyParam()
{
    bool bRet = m_locationTransformation.customApplyParam();
    return bRet;
}

bool Location::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    XML_WRITE_UNIQUE(m_locationId);
    XML_WRITE_UNIQUE(m_locationName);
    bRet &= m_locationTransformation.customSaveParam(p_pDoc, p_pParent);
    return bRet;
}

// -- Behaviour
quint32 Location::getLocationId(void) const
{
    return m_locationId;
}

void Location::setLocationId(quint32 p_value)
{
    m_locationId = p_value;
}


// -- Properties
QString Location::getLocationName(void) const
{
    return m_locationName;
}

void Location::setLocationName(const QString & p_value)
{
    m_locationName = p_value;
}

SystemTransformation Location::getLocationTransformation(void) const
{
    return m_locationTransformation;
}

void Location::setLocationTransformation(const SystemTransformation & p_value)
{
    m_locationTransformation = p_value;
}

// -- operator
Location & Location::operator= (const Location & p_value)
{
    if( this != (&p_value) ) {
        m_locationId = p_value.getLocationId();
        m_locationName = p_value.getLocationName();
        m_locationTransformation = p_value.getLocationTransformation();
    }
    return (*this);
}

bool Location::operator== (const Location & p_value)
{
    return (m_locationId == p_value.getLocationId()) &&
            (m_locationName == p_value.getLocationName()) &&
            (m_locationTransformation == p_value.getLocationTransformation());
}
