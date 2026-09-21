/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlSettings.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlHelper.h>

// -- Qt includes
#include <QString>
#include <QList>
#include <QMutex>

class QDomElement;
class QDomDocument;

/**
 * @brief The XmlSettings class
 *
 * Settings are organized like in a tree :
 * - root
 *      - child 1 : MainWindow Settings
 *          - window position
 *          - window size
 *          ...
 *      - child 2 : last project
 *          - file name of the last project
 *      ...
 *
 * A child is commonly a class. Each child musst subscribe to a parent (or root).
 * When the programm is closed, all classes that have read settings will be called before the program exit to save their parameters
 *
 */
class XmlSettings
{
public:
    XmlSettings(const QString & p_className);   ///< class name is used to generate a section in the xml file
    virtual ~XmlSettings();

    bool loadSettings(QDomElement * p_pParent = nullptr);    ///< load settings of the inherited class. If p_pParent is not specified, it will take 'root' as parent
    bool saveSettings(QDomElement * p_pParent = nullptr);    ///< save settings of the inherited class. If p_pParent is not specified, it will take 'root' as parent

    // -- Static methods
    static bool readSettingsFromFile(); ///< only initialize settings
    static bool writeSettingsToFile();  ///< call all objects that have loaded settings
    void static deleteXmlDoc(); ///< delete m_pXmlRoot and m_pXmlDoc

    // -- Class Name
    void setXmlSettingsName(const QString & p_name);
    QString getXmlSettingsName() const;

protected :
    // -- Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) = 0;   ///< Inherited classes must overload this class, just set variable values from xml
    virtual bool customLoadDefaultSettings() = 0;                 ///< Inherited classes must overload this class, just set default variable values
    virtual bool customApplySettings() = 0;                         ///< Apply settings from variables values
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) = 0; ///< Inherited classes must overload this class


private :   

    // -- Xml members
    static QString m_settingsFileName;  ///< Filename of settings file, set by readSettingsFromFile
    static QRecursiveMutex m_mXmlDoc;   ///< protect concurent use of xml items
    static QDomDocument * m_pXmlDoc;
    static QDomElement * m_pXmlRoot;

    // Behaviour
    static QList <XmlSettings *> m_lChildrenSettings;   ///< object that have load settings, will be called by writeSettingsToFile
    static bool m_bUseDefaultSettings;  ///< settings file not found, or user request a reset of settings
    QString m_className;

};
