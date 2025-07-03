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

// -- Internal includes
#include <xml/XmlHelper.h>

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
        QDomText paramValue = p_pDoc->createTextNode(convertStringToXml(p_value.toString()));
        // add the parameter to the tag
        paramTag.appendChild(paramValue);
        // add the tag to the parent
        p_pParent->appendChild(paramTag);
    } else {
        // Parameter found, change its value with a new element
        QDomElement newParamTag = p_pDoc->createElement(p_parameterName);
        QDomText paramValue = p_pDoc->createTextNode(convertStringToXml(p_value.toString()));
        newParamTag.appendChild(paramValue);
        p_pParent->replaceChild(newParamTag, paramTag);
    }
    return true;
}



