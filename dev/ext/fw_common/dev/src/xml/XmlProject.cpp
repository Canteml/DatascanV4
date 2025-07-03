/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlProject.cpp
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

// -- Internal includes
#include <xml/XmlProject.h>
#include <log/Logger.h>

// -- Qt includes
#include <QFile>
#include <QTextStream>

XmlProject::XmlProject(const QString & p_sectionName)
{
    m_sectionName = p_sectionName;
}

XmlProject::~XmlProject()
{

}

bool XmlProject::loadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    QDomElement xmlSection;

    if( nullptr != p_pParent ) {
        // find section
        xmlSection = XmlHelper::xmlGetChild(p_pParent, m_sectionName);
        if( xmlSection.isNull() ) {
            Logger::logWarning(QString("XmlProject::loadParam : Section \"%1\" not found.").arg(m_sectionName));
        }
    }

    bRet &= loadParamSection(&xmlSection);

    return bRet;
}

bool XmlProject::loadParamSection(QDomElement * p_pSection)
{
    bool bRet = false;
    if( nullptr != p_pSection && !p_pSection->isNull()) {
        bRet = customLoadParam(p_pSection);
        if( !bRet ) {
            Logger::logError(QString("XmlProject::loadParamSection - Failed to load parameters for \"%1\".").arg(m_sectionName));
        }
    }

    if( !bRet ) {
        Logger::logWarning(QString("XmlProject::loadParamSection - Load default parameters for \"%1\".").arg(m_sectionName));
        bRet = customLoadDefaultParam();
        if( !bRet ) {
            Logger::logError(QString("XmlProject::loadParamSection - Failed to load default parameters for \"%1\".").arg(m_sectionName));
        }
    }

    if( bRet ) {
        // apply param
        customApplyParam();
    }

    return bRet;
}

bool XmlProject::loadDefaultParam()
{
    bool bRet = true;
    bRet &= customLoadDefaultParam();
    bRet &= customApplyParam();
    return bRet;
}

bool XmlProject::saveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;

    // use a section in the XML file :
    QDomElement xmlSection = XmlHelper::xmlAddUniqueChild(p_pDoc, p_pParent, m_sectionName);
    bRet = customSaveParam(p_pDoc, &xmlSection);

    return bRet;
}

bool XmlProject::readProjectFromFile(const QString & p_fileName)
{
    bool bRet = true;
    bool bUseDefaultParam = false;

    // Make xml objects
    QDomDocument * pXmlDoc = new QDomDocument();
    QDomElement * pXmlRoot = new QDomElement();

    // open project file
    QFile xmlFile(p_fileName);
    if( !xmlFile.open(QIODevice::ReadOnly) ) {
        Logger::logWarning(QString("XmlProject::readParamFromFile : Fail to load parameters from file (%1).").arg(p_fileName));
        bUseDefaultParam = true;
    } else {
        // Check xml rules in the setting file
        QString xmlError;
        int errLine;
        int errCol;
        if( !pXmlDoc->setContent(&xmlFile, false, &xmlError, &errLine, &errCol) ) {
            // Log error
            Logger::logError(QString("XmlProject::readParamFromFile : Invalid xml file (%1)").arg(p_fileName));
            Logger::logError(QString("XmlProject::readParamFromFile : LINE %1, COLUMN %2 : %3").arg(errLine).arg(errCol).arg(xmlError));
            bUseDefaultParam = true;
            bRet = false;
        } else {
            Logger::logInfo(QString("Load param from : %1").arg(p_fileName));
            // find root element
            (*pXmlRoot) = pXmlDoc->documentElement();
            if( pXmlRoot->isNull() ) {
                Logger::logError("XmlProject::readParamFromFile : no root element");
                bUseDefaultParam = true;
                bRet = false;
            } else {
                if( pXmlRoot->nodeName() != "root" ) {
                    Logger::logError(QString("XmlProject::readParamFromFile : first element must be \"root\", not %1.").arg(pXmlRoot->nodeName()));
                    bUseDefaultParam = true;
                    bRet = false;
                } else {
                    // Valid root !
                    bRet &= loadParam(pXmlRoot);
                }
            }
        }
    }
    xmlFile.close();
    pXmlDoc->clear();
    delete pXmlRoot;
    delete pXmlDoc;


    if( bUseDefaultParam ) {
        bRet = loadParam();
    }

    if( bRet ) {
        m_projectFileName = p_fileName;
    }
    return bRet;
}

///< save Param to file
bool XmlProject::writeProjectToFile(QString p_fileName)
{
    bool bRet = true;

    if( p_fileName.isEmpty() ) {
        p_fileName = m_projectFileName;
    }

    QDomDocument * pXmlDoc = new QDomDocument();
    QDomElement * pXmlRoot = new QDomElement();

    // Write parameters (overwrite any existing file)
    QFile xmlFile(p_fileName);
    if( !xmlFile.open(QIODevice::WriteOnly | QIODevice::Text ) ) {
        Logger::logError(QString("XmlProject::writeProjectToFile : Fail to open project file (%1).").arg(p_fileName));
        bRet = false;
    } else {
        Logger::logInfo(QString("Save project to : %1").arg(p_fileName));
        QTextStream xmlStream(&xmlFile);

        // Make xml header (https://www.alsacreations.com/astuce/lire/34-charset-iso-8859-1-iso-8859-15-utf-8-lequel-choisir.html)
        QDomNode xmlNode = pXmlDoc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        pXmlDoc->appendChild(xmlNode);
        (*pXmlRoot) = pXmlDoc->createElement("root");
        pXmlDoc->appendChild(*pXmlRoot);
        bRet &= saveParam(pXmlDoc, pXmlRoot);

        xmlStream<<pXmlDoc->toString();
    }
    xmlFile.close();

    pXmlDoc->clear();
    delete pXmlRoot;
    delete pXmlDoc;


    if( bRet ) {
        // update filename
        m_projectFileName = p_fileName;
    }

    return bRet;
}

void XmlProject::setXmlSectionName(const QString & p_name)
{
    m_sectionName = p_name;
}

QString XmlProject::getXmlSectionName() const
{
    return m_sectionName;
}

bool XmlProject::customLoadDefaultParam()
{
    return true;
}

bool XmlProject::customApplyParam()
{
    return true;
}

// ----------------------------------------------------------------------------
// -- Class XmlProjectLoggable

XmlProjectLoggable::XmlProjectLoggable(const QString & p_name) : Loggable(p_name), XmlProject(p_name)
{

}

XmlProjectLoggable::~XmlProjectLoggable()
{

}
