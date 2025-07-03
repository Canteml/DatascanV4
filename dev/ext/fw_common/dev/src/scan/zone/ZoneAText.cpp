/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ZoneAText.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/zone/ZoneAText.h>

ZoneAText::ZoneAText() : ZoneAnnotation("ZoneAText")
{

}

ZoneAText::~ZoneAText()
{

}


// -- XmlProject
bool ZoneAText::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_text);
    bRet &= ZoneAnnotation::customLoadParam(p_pParent);
    return bRet;
}

bool ZoneAText::customLoadDefaultParam()
{
    bool bRet = true;
    m_text = "";
    bRet &= ZoneAnnotation::customLoadDefaultParam();
    return bRet;
}

bool ZoneAText::customApplyParam()
{
    bool bRet = true;
    bRet &= ZoneAnnotation::customApplyParam();
    return bRet;
}

bool ZoneAText::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_text);
    bRet &= ZoneAnnotation::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


// -- Properties
QString ZoneAText::getText(void) const
{
    return m_text;
}

void ZoneAText::setText(const QString& p_value)
{
    m_text = p_value;
}
