/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : SystemTransformation.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/type/SystemTransformation.h>


// ----------------------------------------------------------------------------
// -- Class SystemTransformation

SystemTransformation::SystemTransformation() : XmlProjectLoggable("SystemTransformation")
{
    m_translate = QPointF(0,0);
    m_rotate = 0;
}

SystemTransformation::SystemTransformation(const SystemTransformation& p_value) : XmlProjectLoggable("SystemTransformation")
{
    (*this) = p_value;
}

// -- XmlProject
bool SystemTransformation::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;

    QString _translate;
    bRet &= XML_READ_UNIQUE(QString, _translate);
    m_translate = XmlHelper::QPointFFromString(_translate);
    bRet &= XML_READ_UNIQUE(qreal, m_rotate);
    return bRet;
}

bool SystemTransformation::customLoadDefaultParam()
{
    bool bRet = true;
    m_translate = QPointF(0,0);
    m_rotate = 0;
    return bRet;
}

bool SystemTransformation::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool SystemTransformation::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString _translate = XmlHelper::QPointFToString(m_translate);
    bRet &= XML_WRITE_UNIQUE(_translate);
    bRet &= XML_WRITE_UNIQUE(m_rotate);
    return bRet;
}

// -- operator
SystemTransformation & SystemTransformation::operator= (const SystemTransformation & p_value)
{
    if( this != (&p_value) ){
           m_translate = p_value.m_translate;
           m_rotate = p_value.m_rotate;
    }
    return (*this);
}

bool SystemTransformation::operator== (const SystemTransformation & p_value)
{
    return (p_value.m_translate == m_translate) && (p_value.m_rotate == m_rotate);
}
