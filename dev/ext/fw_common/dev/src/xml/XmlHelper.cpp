/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlHelper.cpp
# Author      : Thierry Lechevallier
# Date        : 27/05/2020
#
# ==========================================================================*/

// -- Qt includes
#include <QDomDocument>
#include <QDomElement>
#include <QPointF>
#include <QSizeF>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QRectF>

// -- Internal includes
#include <xml/XmlHelper.h>
#include <log/Logger.h>

// ----------------------------------------------------------------------------
// -- Namespace XmlHelper

QString XmlHelper::convertStringToXml(const QString & p_string)
{
    QString sRet = p_string;
    // https://www.freecodecamp.org/forum/t/convert-html-entities-string-replace/150382
    sRet.replace ("&", "&amp;");
    sRet.replace ("<", "&lt;");
    sRet.replace (">", "&gt;");
    sRet.replace ("\'", "&quot;");
    sRet.replace ("\"", "&apos;");
    // https://stackoverflow.com/questions/27826527/xmlstreamwriter-does-not-convert-n-to-xa-when-writing-attributes
    sRet.replace ("\r", "&#xD;");
    sRet.replace ("\n", "&#xA;");
    sRet.replace ("é", "&#233;");
    sRet.replace ("ê", "&#234;");
    sRet.replace ("è", "&#232;");
    sRet.replace ("à", "&#224;");
    sRet.replace ("É", "&#201;");
    sRet.replace ("Ê", "&#202;");
    sRet.replace ("È", "&#200;");
    return sRet;
}

QString XmlHelper::convertXmlToString(const QString & p_string)
{
    QString sRet = p_string;
    // https://www.freecodecamp.org/forum/t/convert-html-entities-string-replace/150382
    sRet.replace ("&amp;", "&");
    sRet.replace ("&lt;", "<");
    sRet.replace ("&gt;", ">");
    sRet.replace ("&quot;", "\'");
    sRet.replace ("&apos;", "\"");
    // https://stackoverflow.com/questions/27826527/xmlstreamwriter-does-not-convert-n-to-xa-when-writing-attributes
    sRet.replace ("&#xD;", "\r");
    sRet.replace ("&#xA;", "\n");
    sRet.replace ("&#233;", "é");
    sRet.replace ("&#234;", "ê");
    sRet.replace ("&#232;", "è");
    sRet.replace ("&#224;", "à");
    sRet.replace ("&#201;", "É");
    sRet.replace ("&#202;", "Ê");
    sRet.replace ("&#200;", "È");
    return sRet;
}

QDomElement XmlHelper::xmlAddChild(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_childName)
{
    QDomElement child;
    child = p_pDoc->createElement(p_childName);
    p_pParent->appendChild(child);
    return child;
}


QDomElement XmlHelper::xmlAddUniqueChild(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_childName)
{
    QDomElement child = xmlGetChild(p_pParent, p_childName);
    if( child.isNull() ) {
        child = xmlAddChild(p_pDoc, p_pParent, p_childName);
    }
    return child;
}


QDomElement XmlHelper:: xmlGetChild(QDomElement * p_pParent, const QString & p_childName)
{
    return p_pParent->firstChildElement(p_childName);
}

bool XmlHelper::xmlWriteUniqueParameter(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_parameterName, const QVariant & p_value)
{
    // Find if a parameter aldready exist with this name

    QDomElement paramTag = p_pParent->firstChildElement(p_parameterName);
    if( paramTag.isNull() ) {
        // Parameter doesn't exist, make it !
        paramTag = p_pDoc->createElement(p_parameterName);
        QDomText paramValue = p_pDoc->createTextNode(convertVariantToXml(p_value));
        // add the parameter to the tag
        paramTag.appendChild(paramValue);
        // add the tag to the parent
        p_pParent->appendChild(paramTag);
    } else {
        // Parameter found, change its value with a new element
        QDomElement newParamTag = p_pDoc->createElement(p_parameterName);
        QDomText paramValue = p_pDoc->createTextNode(convertVariantToXml(p_value));
        newParamTag.appendChild(paramValue);
        p_pParent->replaceChild(newParamTag, paramTag);
    }
    return true;
}

QString XmlHelper::convertVariantToXml(const QVariant &p_value)
{
    // UChar is converted in string as unicode : 4 => "\004". The decode method can't get int with unicode...
    if( p_value.type() == (QVariant::Type)QMetaType::UChar ) {
        QVariant v;
        v.setValue(p_value.toInt());
        return convertStringToXml(v.toString());
    }
    return convertStringToXml(p_value.toString());
}

QString XmlHelper::QPointFToString(const QPointF& p_value)
{
    return QString("%1;%2").arg(p_value.x()).arg(p_value.y());
}

QPointF XmlHelper::QPointFFromString(const QString & p_value)
{
    QPointF ret;
    QStringList l = p_value.split(";");
    if( 2 == l.size() ) {
        ret.setX(l.at(0).toDouble());
        ret.setY(l.at(1).toDouble());
    } else {
        Logger::logError("XmlHelper::QPointFFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}


QString XmlHelper::QPointToString(const QPoint& p_value)
{
    return QString("%1;%2").arg(p_value.x()).arg(p_value.y());
}

QPoint XmlHelper::QPointFromString(const QString& p_value)
{
    QPoint ret;
    QStringList l = p_value.split(";");
    if( 2 == l.size() ) {
        ret.setX(l.at(0).toDouble());
        ret.setY(l.at(1).toDouble());
    } else {
        Logger::logError("XmlHelper::QPointFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}


QString XmlHelper::QSizeFToString(const QSizeF& p_value)
{
    return QString("%1;%2").arg(p_value.width()).arg(p_value.height());
}

QSizeF XmlHelper::QSizeFFromString(const QString & p_value)
{
    QSizeF ret;
    QStringList l = p_value.split(";");
    if( 2 == l.size() ) {
        ret.setWidth(l.at(0).toDouble());
        ret.setHeight(l.at(1).toDouble());
    } else {
        Logger::logError("XmlHelper::QSizeFFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}


QString XmlHelper::QSizeToString(const QSize& p_value)
{
    return QString("%1;%2").arg(p_value.width()).arg(p_value.height());
}

QSize XmlHelper::QSizeFromString(const QString & p_value)
{
    QSize ret;
    QStringList l = p_value.split(";");
    if( 2 == l.size() ) {
        ret.setWidth(l.at(0).toDouble());
        ret.setHeight(l.at(1).toDouble());
    } else {
        Logger::logError("XmlHelper::QSizeFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}

QString XmlHelper::QRectFToString(const QRectF& p_value)
{
    return QString("%1;%2;%3;%4").arg(p_value.topLeft().x()).arg(p_value.topLeft().y()).arg(p_value.width()).arg(p_value.height());
}

QRectF XmlHelper::QRectFFromString(const QString & p_value)
{
    QRectF ret;
    QStringList l = p_value.split(";");
    if( 4 == l.size() ) {
        ret.setTopLeft(QPointF(l.at(0).toDouble(), l.at(1).toDouble()));
        ret.setWidth(l.at(2).toDouble());
        ret.setHeight(l.at(3).toDouble());
    } else {
        Logger::logError("XmlHelper::QRectFFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}

QString XmlHelper::QRectToString(const QRect& p_value)
{
    return QString("%1;%2;%3;%4").arg(p_value.topLeft().x()).arg(p_value.topLeft().y()).arg(p_value.width()).arg(p_value.height());
}

QRect XmlHelper::QRectFromString(const QString & p_value)
{
    QRect ret;
    QStringList l = p_value.split(";");
    if( 4 == l.size() ) {
        ret.setTopLeft(QPoint(l.at(0).toInt(), l.at(1).toInt()));
        ret.setWidth(l.at(2).toInt());
        ret.setHeight(l.at(3).toInt());
    } else {
        Logger::logError("XmlHelper::QRectFromString - String must have a list with ';' as separator : " + p_value);
    }
    return ret;
}


