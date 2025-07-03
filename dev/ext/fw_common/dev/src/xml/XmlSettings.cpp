/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlSettings.cpp
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

// -- Internal includes
#include <xml/XmlSettings.h>
#include <log/Logger.h>
#include <core/Core.h>
#include <xml/XmlHelper.h>

// -- Qt includes
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>


// -- Xml file
QString XmlSettings::m_settingsFileName = "";
QMutex XmlSettings::m_mXmlDoc(QMutex::Recursive);
QDomDocument * XmlSettings::m_pXmlDoc = nullptr;
QDomElement * XmlSettings::m_pXmlRoot = nullptr;

// Behaviour
QList <XmlSettings *> XmlSettings::m_lChildrenSettings;
bool XmlSettings::m_bUseDefaultSettings = false;


XmlSettings::XmlSettings(const QString & p_className)
{
    m_className = p_className;
}

XmlSettings::~XmlSettings()
{
    m_lChildrenSettings.removeAll(this);
}

void XmlSettings::deleteXmlDoc()
{
    m_mXmlDoc.lock();
    if( nullptr != m_pXmlRoot ) {
        delete m_pXmlRoot;
        m_pXmlRoot = nullptr;
    }
    if( nullptr != m_pXmlDoc ) {
        delete m_pXmlDoc;
        m_pXmlDoc = nullptr;
    }
    m_mXmlDoc.unlock();
}

bool XmlSettings::loadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    m_mXmlDoc.lock();

    if( nullptr == p_pParent ) {
        p_pParent = m_pXmlRoot;
    }

    // add this class to the list to be saved at the program exit
    if( !m_lChildrenSettings.contains(this) && (p_pParent == m_pXmlRoot) ) {
        m_lChildrenSettings.append(this);
    }

    // load settings
    if( !m_bUseDefaultSettings ) {
        // get section
        QDomElement xmlSection = XmlHelper::xmlGetChild(p_pParent, m_className);
        if( xmlSection.isNull() ) {
            Logger::logError(QString("XmlSettings::loadSettings : Section \"%1\" not found.").arg(m_className));
            bRet = false;
        } else {
            bRet = customLoadSettings(&xmlSection);
        }

        if( !bRet ) {
            Logger::logWarning(QString("XmlSettings::loadSettings : beacause of errors, load default parameters for \"%1\".").arg(m_className));
            bRet = customLoadDefaultSettings();
            if( !bRet ) {
                Logger::logError(QString("Failed to load default settings for \"%1\".").arg(m_className));
            }
        }
    } else {
        bRet = customLoadDefaultSettings();
    }
    m_mXmlDoc.unlock();

    // apply settings
    customApplySettings();

    return bRet;
}

bool XmlSettings::saveSettings(QDomElement * p_pParent)
{    
    bool bRet = true;
    m_mXmlDoc.lock();

    if( nullptr == p_pParent ) {
        p_pParent = m_pXmlRoot;
    } else {
        // settings managed by a "parent". Do not write them in the "root" master tag
        m_lChildrenSettings.removeAll(this);
    }

    // use a section in the XML file :
    QDomElement xmlSection = XmlHelper::xmlAddUniqueChild(m_pXmlDoc, p_pParent, m_className);
    bRet = customSaveSettings(m_pXmlDoc, &xmlSection);

    m_mXmlDoc.unlock();

    return bRet;
}

// -- methods
bool XmlSettings::readSettingsFromFile()
{
    bool bRet = true;
    m_mXmlDoc.lock();

    // Settings already loaded ?
    if( m_pXmlDoc != nullptr || m_pXmlRoot != nullptr ) {
        deleteXmlDoc();
    }

    // Make xml objects
    m_pXmlDoc = new QDomDocument();
    m_pXmlRoot = new QDomElement();

    // compute settings filename
    m_settingsFileName = QString("%1/settings.xml").arg(Core::getDirDataConf());
    QFile xmlFile(m_settingsFileName);
    if( !xmlFile.open(QIODevice::ReadOnly) ) {
        Logger::logWarning(QString("XmlSettings::readSettingsFromFile : Fail to load settings from file (%1).").arg(m_settingsFileName));
        m_bUseDefaultSettings = true;
    } else {
        // Check xml rules in the setting file
        QString xmlError;
        int errLine;
        int errCol;
        if( !m_pXmlDoc->setContent(&xmlFile, false, &xmlError, &errLine, &errCol) ) {
            // Log error
            Logger::logError(QString("XmlSettings::readSettingsFromFile : Invalid xml file (%1)").arg(m_settingsFileName));
            Logger::logError(QString("XmlSettings::readSettingsFromFile : LINE %1, COLUMN %2 : %3").arg(errLine).arg(errCol).arg(xmlError));
            m_bUseDefaultSettings = true;
            bRet = false;
        } else {
            Logger::logInfo(QString("Load settings from : %1").arg(m_settingsFileName));
            // find root element
            (*m_pXmlRoot) = m_pXmlDoc->documentElement();
            if( m_pXmlRoot->isNull() ) {
                Logger::logError("XmlSettings::readSettingsFromFile : no root element");
                m_bUseDefaultSettings = true;
                bRet = false;
            } else {
                if( m_pXmlRoot->nodeName() != "root" ) {
                    Logger::logError(QString("XmlSettings::readSettingsFromFile : first element must be \"root\", not %1.").arg(m_pXmlRoot->nodeName()));
                    m_bUseDefaultSettings = true;
                    bRet = false;
                } else {
                    // Valid root !
                }
            }
        }
    }
    xmlFile.close();

    if( m_bUseDefaultSettings ) {
        Logger::logWarning("XmlSettings::readSettingsFromFile : use default settings");
        // Make xml from scrach
        deleteXmlDoc();
        m_pXmlDoc = new QDomDocument();
        m_pXmlRoot = new QDomElement();

        // Make xml header (https://www.alsacreations.com/astuce/lire/34-charset-iso-8859-1-iso-8859-15-utf-8-lequel-choisir.html)
        QDomNode xmlNode = m_pXmlDoc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        m_pXmlDoc->appendChild(xmlNode);
        (*m_pXmlRoot) = m_pXmlDoc->createElement("root");
        m_pXmlDoc->appendChild(*m_pXmlRoot);
    }

    m_mXmlDoc.unlock();
    return bRet;
}
bool XmlSettings::writeSettingsToFile()
{
    bool bRet = true;
    m_mXmlDoc.lock();

    // settings object initialized ?
    if( m_pXmlDoc != nullptr ) {
        // Ask to all registered objects to save their settings
        for(auto i : qAsConst(m_lChildrenSettings)) {
            bRet &= i->saveSettings();
        }

        // Write settings (overwrite any existing file)
        QFile xmlFile(m_settingsFileName);
        if( !xmlFile.open(QIODevice::WriteOnly | QIODevice::Text ) ) {
            Logger::logError(QString("XmlSettings::writeSettingsToFile : Fail to open settings file (%1).").arg(m_settingsFileName));
            bRet = false;
        } else {
            Logger::logInfo(QString("Save settings to : %1").arg(m_settingsFileName));
            QTextStream xmlStream(&xmlFile);
            xmlStream<<m_pXmlDoc->toString();
        }
        xmlFile.close();
    }
    m_mXmlDoc.unlock();
    return bRet;
}

void XmlSettings::setXmlSettingsName(const QString &p_name)
{
    m_className = p_name;
}

QString XmlSettings::getXmlSettingsName() const
{
    return m_className;
}

