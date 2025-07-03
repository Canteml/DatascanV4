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
#include <scan/zone/Zone.h>


// ----------------------------------------------------------------------------
// -- Class Zone


Zone::Zone() : XmlProjectLoggable("Zone")
{
    m_zoneType = TYPE_ACCESSIBLE;
    m_zoneOriginalThickness = 0;
}

Zone::~Zone()
{

}

Zone *Zone::copy()
{
    Zone * pRet = new Zone();
    pRet->setZoneType(m_zoneType);
    pRet->setZoneTitle(m_zoneTitle);
    pRet->setZoneTransfromation(m_zoneTransformation);
    pRet->setZoneSize(m_zoneSize);
    pRet->setZoneResolution(m_zoneResolution);
    pRet->setZoneXAxisTitle(m_zoneXAxisTitle);
    pRet->setZoneYAxisTitle(m_zoneYAxisTitle);
    pRet->setZoneOriginalThickness(m_zoneOriginalThickness);

    // -- Composition : \todo
    return pRet;
}


// -- Behaviour
quint32 Zone::getZoneType(void) const
{
    return m_zoneType;
}

void Zone::setZoneType(quint32 p_value)
{
    m_zoneType = p_value;
}


// -- Parameters
QString Zone::getZoneTitle(void) const
{
    return m_zoneTitle;
}

void Zone::setZoneTitle(const QString& p_value)
{
    m_zoneTitle = p_value;
}

SystemTransformation Zone::getZoneTransformation(void) const
{
    return m_zoneTransformation;
}

void Zone::setZoneTransfromation(const SystemTransformation& p_value)
{
    m_zoneTransformation = p_value;
}

QSizeF Zone::getZoneSize(void) const
{
    return m_zoneSize;
}

void Zone::setZoneSize(const QSizeF& p_value)
{
    m_zoneSize = p_value;
}

QSizeF Zone::getZoneResolution(void) const
{
    return m_zoneResolution;
}

void Zone::setZoneResolution(const QSizeF& p_value)
{
    m_zoneResolution = p_value;
}

QString Zone::getZoneXAxisTitle(void) const
{
    return  m_zoneXAxisTitle;
}

void Zone::setZoneXAxisTitle(const QString& p_value)
{
    m_zoneXAxisTitle = p_value;
}

QString Zone::getZoneYAxisTitle(void) const
{
    return m_zoneYAxisTitle;
}

void Zone::setZoneYAxisTitle(const QString& p_value)
{
    m_zoneYAxisTitle = p_value;
}

qreal Zone::getZoneOriginalThickness(void) const
{
    return m_zoneOriginalThickness;
}

void Zone::setZoneOriginalThickness(qreal p_value)
{
    m_zoneOriginalThickness = p_value;
}


// -- Composition

// -- XmlProject
bool Zone::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(quint32, m_zoneType);
    bRet &= XML_READ_UNIQUE(QString, m_zoneTitle);
    bRet &= m_zoneTransformation.customLoadParam(p_pParent);
    QString _zoneSize, _zoneResolution;

    bRet &= XML_READ_UNIQUE(QString, _zoneSize);
    m_zoneSize = XmlHelper::QSizeFromString(_zoneSize);
    bRet &= XML_READ_UNIQUE(QString, _zoneResolution);
    m_zoneResolution = XmlHelper::QSizeFromString(_zoneResolution);
    bRet &= XML_READ_UNIQUE(QString, m_zoneXAxisTitle);
    bRet &= XML_READ_UNIQUE(QString, m_zoneYAxisTitle);
    bRet &= XML_READ_UNIQUE(qreal, m_zoneOriginalThickness);
    return bRet;
}

bool Zone::customLoadDefaultParam()
{
    bool bRet = true;
    m_zoneType = Zone::ZoneType::TYPE_ACCESSIBLE;
    m_zoneTitle = "";
    bRet &= m_zoneTransformation.customLoadDefaultParam();
    m_zoneSize = QSize(0,0);
    m_zoneResolution = QSize(0,0);
    m_zoneXAxisTitle = "";
    m_zoneYAxisTitle = "";
    m_zoneOriginalThickness = 0;
    return bRet;
}

bool Zone::customApplyParam()
{
    bool bRet = m_zoneTransformation.customApplyParam();
    return bRet;
}

bool Zone::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_zoneType);
    bRet &= XML_WRITE_UNIQUE(m_zoneTitle);
    bRet &= m_zoneTransformation.customSaveParam(p_pDoc, p_pParent);

    QString _zoneSize, _zoneResolution;

    _zoneSize = XmlHelper::QSizeFToString(m_zoneSize);
    bRet &= XML_WRITE_UNIQUE(_zoneSize);
    _zoneResolution = XmlHelper::QSizeFToString(m_zoneResolution);
    bRet &= XML_WRITE_UNIQUE(_zoneResolution);
    bRet &= XML_WRITE_UNIQUE(m_zoneXAxisTitle);
    bRet &= XML_WRITE_UNIQUE(m_zoneYAxisTitle);
    bRet &= XML_WRITE_UNIQUE(m_zoneOriginalThickness);
    return bRet;
}
