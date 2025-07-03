/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlHelper.h
# Author      : Thierry Lechevallier
# Date        : 27/05/2020
#
# ==========================================================================*/

#pragma once

// -- Qt includes
#include <QString>
#include <QVariant>
#include <QDomElement>
class QDomDocument;

// ----------------------------------------------------------------------------
// -- Namespace XmlHelper
/**
 * @brief The XmlHelper namespace
 *
 * This namespace contains static methods used to handle XML
 *
 */
namespace XmlHelper
{

    QString convertStringToXml(const QString & p_string); ///< should be use each time a string will be written in an xml file.
    QString convertXmlToString(const QString & p_string);
    QDomElement xmlAddChild(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_childName);  ///< Return the new node
    QDomElement xmlAddUniqueChild(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_childName);  ///< Return the new node, create it only if not present in parent
    QDomElement xmlGetChild(QDomElement * p_pParent, const QString & p_childName); ///< Return the p_childName node if it exists
    bool xmlWriteUniqueParameter(QDomDocument * p_pDoc, QDomElement * p_pParent, const QString & p_parameterName, const QVariant & p_value);

    template <typename T> bool xmlReadUniqueParameter(QDomElement * p_pParent, T & p_variable, const QString & p_parameterName) {
        bool bRet = false;
        QDomElement element = p_pParent->firstChildElement(p_parameterName);
        if( !element.isNull() ) {
            QString val = element.text();
            QVariant v(val);
            bRet = v.canConvert<T>();
            if( bRet ) {
                p_variable = v.value<T>();
                // test if convertion is good :
                bRet = QVariant(p_variable).toString() == val;
            }
        }
        return bRet;
    }



};

// ----------------------------------------------------------------------------
// -- Macro

#define XML_WRITE_UNIQUE(VARIABLE)  XmlHelper::xmlWriteUniqueParameter(p_pDoc, p_pParent, #VARIABLE, QVariant::fromValue(VARIABLE))
#define XML_READ_UNIQUE(T, VARIABLE) ({ \
                                        bool bRet = XmlHelper::xmlReadUniqueParameter<T>(p_pParent, VARIABLE, #VARIABLE); \
                                        if( !bRet ) {Logger::logError(QString("XML_READ_UNIQUE - error reading parameter %1").arg(#VARIABLE)); } \
                                        bRet; \
                                      })

