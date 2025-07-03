/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Coupling.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/equipment/Coupling.h>

// -- Qt includes

Coupling::Coupling() : XmlProjectLoggable("Coupling")
{

}

Coupling::Coupling(const Coupling &p_value) : QObject(), XmlProjectLoggable("Coupling")
{
    (*this) = p_value;
}

Coupling::~Coupling()
{

}

bool Coupling::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_paramType);
    bRet &= XML_READ_UNIQUE(QString, m_paramBrand);
    bRet &= XML_READ_UNIQUE(QString, m_paramExpirationDate);
    return bRet;
}

bool Coupling::customLoadDefaultParam()
{
    m_paramType = QString(tr("EAU"));
    m_paramBrand = "-";
    m_paramExpirationDate = "-";
    return true;
}

bool Coupling::customApplyParam()
{
    // Nothing to do
    return true;
}

bool Coupling::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramType);
    bRet &= XML_WRITE_UNIQUE(m_paramBrand);
    bRet &= XML_WRITE_UNIQUE(m_paramExpirationDate);
    return bRet;
}

QString Coupling::getParamType() const
{
    return m_paramType;
}

void Coupling::setParamType(const QString & p_type)
{
    m_paramType = p_type;
}

QString Coupling::getParamBrand(void) const
{
    return m_paramBrand;

}

void Coupling::setParamBrand(const QString & p_brand)
{
    m_paramBrand = p_brand;
}

QString Coupling::getParamExpirationDate(void) const
{
    return m_paramExpirationDate;
}

void Coupling::setParamExpirationDate(const QString & p_expirationDate)
{
    m_paramExpirationDate = p_expirationDate;
}

Coupling &Coupling::operator=(const Coupling &p_value)
{
    if( this != &p_value ) {
        setParamType(p_value.getParamType());
        setParamBrand(p_value.getParamBrand());
        setParamExpirationDate(p_value.getParamExpirationDate());
    }
    return (*this);
}

bool Coupling::operator==(const Coupling &p_value)
{
    return (p_value.getParamType() == m_paramType &&
            p_value.getParamBrand() == m_paramBrand &&
            p_value.getParamExpirationDate() == m_paramExpirationDate);
}
