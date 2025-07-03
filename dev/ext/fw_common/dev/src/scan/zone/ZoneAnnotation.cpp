/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ZoneAnnotation.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/


// -- Internal includes
#include <scan/zone/ZoneAnnotation.h>

// -- Qt includes

ZoneAnnotation::ZoneAnnotation(const QString& p_annotationName) : XmlProjectLoggable(p_annotationName)
{

}

ZoneAnnotation::~ZoneAnnotation()
{

}


// -- XmlProject
bool ZoneAnnotation::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    QString _position = XmlHelper::QPointFToString(m_position);
    bRet &= XML_READ_UNIQUE(QString, _position);
    bRet &= XML_READ_UNIQUE(QColor, m_color); // \todo QColor doesn't work
    return bRet;
}

bool ZoneAnnotation::customLoadDefaultParam()
{
    m_position = QPointF(0,0);
    m_color = QColor(0,0,0);
    return true;
}

bool ZoneAnnotation::customApplyParam()
{
    return true;
}

bool ZoneAnnotation::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString _position;
    bRet &= XML_WRITE_UNIQUE(_position);
    m_position = XmlHelper::QPointFromString(_position);
    bRet &= XML_WRITE_UNIQUE(m_color);
    return bRet;
}

// -- Properties
QPointF ZoneAnnotation::getPosition(void) const
{
    return m_position;
}

void ZoneAnnotation::setPosition(const QPoint& p_value)
{
    m_position = p_value;
}

QColor ZoneAnnotation::getColor(void) const
{
    return m_color;
}

void ZoneAnnotation::setColor(const QColor & p_value)
{
    m_color = p_value;
}

