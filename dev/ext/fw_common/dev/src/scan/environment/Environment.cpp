/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Environment.cpp
# Author      : Thierry Lechevallier
# Date        : 28/10/2020
#
# ==========================================================================*/

#include <scan/environment/Environment.h>
enum EnvOperating {
    ENV_NOT_OPERATING,
    ENV_OPERATING
};

Environment::Environment() : XmlProjectLoggable("Environment")
{
    m_operating = ENV_NOT_OPERATING;
}

Environment::Environment(const Environment & p_value) : QObject(), XmlProjectLoggable("Environment")
{
    (*this) = p_value;
}

Environment::~Environment()
{

}

// -- Properties
void Environment::setOperating(quint32 p_value)
{
    m_operating = p_value;
}

quint32 Environment::getOperating() const
{
    return m_operating;
}


// -- operators
bool Environment::operator==(const Environment & p_value)
{
    return m_operating == p_value.getOperating();

}

Environment & Environment::operator=(const Environment & p_value)
{
    if( this != (&p_value ) ) {
        m_operating = p_value.getOperating();
    }
    return (*this);
}

// -- XmlSettings
bool Environment::customLoadParam(QDomElement * p_pParent)
{
    return XML_READ_UNIQUE(quint32, m_operating);
}

bool Environment::customLoadDefaultParam()
{
    m_operating = ENV_NOT_OPERATING;
    return true;
}

bool Environment::customApplyParam()
{
    return true;
}

bool Environment::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    return XML_WRITE_UNIQUE(m_operating);
}
