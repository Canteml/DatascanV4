/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Operator.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#include <scan/environment/Operator.h>

Operator::Operator() : XmlProjectLoggable("Operator")
{

}

Operator::Operator(const Operator & p_value) : QObject(), XmlProjectLoggable("Operator")
{
    (*this) = p_value;
}

Operator::~Operator()
{
    // nothing to delete;
}


// -- Properties
QString Operator::getFirstName() const
{
    return m_firstName;
}

void Operator::setFirstName(const QString & p_value)
{
    m_firstName = p_value;
}

QString Operator::getLastName(void) const
{
    return m_lastName;
}

void Operator::setLastName(const QString & p_value)
{
    m_lastName = p_value;
}

QString Operator::getCardNumber(void) const
{
    return m_cardNumber;
}

void Operator::setCardNumber(const QString & p_value)
{
    m_cardNumber = p_value;
}


// -- operators
bool Operator::operator==(const Operator & p_value)
{
    return m_firstName == p_value.getFirstName() &&
            m_lastName == p_value.getLastName() &&
            m_cardNumber == p_value.getCardNumber();
}

Operator & Operator::operator=(const Operator & p_value)
{
    if( this != (&p_value) ) {
        m_firstName = p_value.getFirstName();
        m_lastName = p_value.getLastName();
        m_cardNumber = p_value.getCardNumber();
    }
    return (*this);
}


// -- XmlSettings
bool Operator::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    XML_READ_UNIQUE(QString, m_firstName);
    XML_READ_UNIQUE(QString, m_lastName);
    XML_READ_UNIQUE(QString, m_cardNumber);
    return bRet;
}

bool Operator::customLoadDefaultParam()
{
    bool bRet = true;
    m_firstName = "";
    m_lastName = "";
    m_cardNumber = "";
    return bRet;
}

bool Operator::customApplyParam()
{
    bool bRet = true;
    // nothing to do
    return bRet;
}

bool Operator::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_firstName);
    bRet &= XML_WRITE_UNIQUE(m_lastName);
    bRet &= XML_WRITE_UNIQUE(m_cardNumber);
    return bRet;
}
