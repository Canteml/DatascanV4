/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlProject.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlHelper.h>
#include <log/Loggable.h>

// -- Qt includes
#include <QString>
#include <QList>
#include <QMutex>

class QDomElement;
class QDomDocument;

// ----------------------------------------------------------------------------
// -- Class XmlProject


/**
 * @brief The XmlProject class
 *
 */
class XmlProject
{
public:
    XmlProject(const QString & p_sectionName);   ///< class name is used to generate a section in the xml file
    virtual ~XmlProject();

    bool loadParam(QDomElement * p_pParent = nullptr);              ///< load params from a parent section. If p_pParent is null, default param are used
    bool loadParamSection(QDomElement * p_pSection = nullptr);                ///< load params from the section (usefull for list of items).
    bool loadDefaultParam();
    bool saveParam(QDomDocument * p_pDoc, QDomElement * p_pParent); ///< save param of the inherited class.

    // -- Project methods
    bool readProjectFromFile(const QString & p_fileName);          ///< load Param from file
    bool writeProjectToFile(QString p_fileName = "");   ///< save Param from file. If p_fileName is empty, we take thr name used for loading

    // -- Class Name
    void setXmlSectionName(const QString & p_name);
    QString getXmlSectionName() const;

protected :
    // -- Inherited behaviour
    virtual bool customLoadParam(QDomElement * p_pParent) = 0;      ///< Inherited classes must overload this class, just set variable values from xml
    virtual bool customLoadDefaultParam();                          ///< Inherited classes must overload this class, just set default variable values
    virtual bool customApplyParam();                                ///< Apply param from variables values
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) = 0; ///< Inherited classes must overload this class

private :

    // -- Xml members
    QString m_projectFileName;    ///< Filename of project file, set by readProjectFromFile

    // Behaviour
    QString m_sectionName;
};

// ----------------------------------------------------------------------------
// -- Class XmlProjectLoggable

/**
 * @brief The XmlProjectLoggable class
 *
 * Convenient Class to inherit from XmlProject and Loggable
 *
 */
class XmlProjectLoggable : public Loggable, public XmlProject
{
public:
    explicit XmlProjectLoggable(const QString & p_name);
    virtual ~XmlProjectLoggable();
};

