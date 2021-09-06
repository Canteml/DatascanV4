/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamManager.cpp
# Author      : Thierry Lechevallier
# Date        : 02/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include "ExamManager.h"

// -- Qt Includes
#include <QStandardPaths>
#include <QtMath>

ExamManager* ExamManager::m_pMyInstance = nullptr;

ExamManager::ExamManager() : XmlSettingsLoggable("ExamManager")
{
    m_bscanInterval = 0;
    m_mepIntervalX = 0;
    m_mepIntervalY = 0;
    m_currentBScanGeneratice = 0;
}

ExamManager::~ExamManager()
{
    m_pMyInstance = nullptr;
}

ExamManager* ExamManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("ExamManager::makeManager : manager already instanciated");
    } else {
        m_pMyInstance = new ExamManager();
    }
    return m_pMyInstance;
}

ExamManager *ExamManager::getInstance()
{
    if( nullptr == m_pMyInstance ) {
        Logger::logError("ExamManager::getInstance : manager not instanciated");
    }
    return m_pMyInstance;
}

void ExamManager::initManager()
{
    // empty
}

// -- Properties

// -- Properties
QString ExamManager::getStructName(void) const
{
    return m_structName;
}

void ExamManager::setStructName(const QString& p_value)
{
    if( m_structName != p_value ) {
        m_structName = p_value;
        updateCurrentExamName();
        emit evStructNameChanged(m_structName);
    }
}

QString ExamManager::getPieceName(void) const
{
    return m_pieceName;
}

void ExamManager::setPieceName(const QString& p_value)
{
    if( m_pieceName != p_value ) {
        m_pieceName = p_value;
        updateCurrentExamName();
        emit evPieceNameChanged(m_pieceName);
    }
}

QString ExamManager::getExportFolder(void) const
{
    return m_exportFolder;
}

void ExamManager::setExportFolder(const QString& p_value)
{
    if( m_exportFolder != p_value ) {
        m_exportFolder = p_value;
        emit evExportFolderChanged(m_exportFolder);
    }
}

quint32 ExamManager::getControlType(void) const
{
    return m_controlType;
}

void ExamManager::setControlType(quint32 p_value)
{
    if( m_controlType != p_value ) {
        m_controlType = p_value;       
        emit evControlTypeChanged(m_controlType);
        updateCurrentExamName();
    }
}

quint32 ExamManager::getBScanNbGeneratrice(void) const
{
    return m_bscanNbGeneratrice;
}

void ExamManager::setBScanNbGeneratrice(quint32 p_value)
{
    if( m_bscanNbGeneratrice != p_value ) {
        m_bscanNbGeneratrice = p_value;
        emit evBScanNbGeneratriceChanged(m_bscanNbGeneratrice);
        updateBScanInterval();
    }
}

qreal ExamManager::getBScanInterval(void) const
{
    return m_bscanInterval;
}

void ExamManager::setBScanInterval(qreal p_value)
{
    if( m_bscanInterval != p_value ) {
        m_bscanInterval = p_value;
        emit evBScanIntervalChanged(m_bscanInterval);
    }
}

qreal ExamManager::getBScanPerimeter(void) const
{
    return m_bscanPerimeter;
}

void ExamManager::setBScanPerimeter(qreal p_value)
{
    if( m_bscanPerimeter != p_value ) {
        m_bscanPerimeter = p_value;
        m_bscanDiameter = m_bscanPerimeter/M_PI;
        emit evBScanDiameterChanged(m_bscanDiameter);
        emit evBScanPerimeterChanged(m_bscanPerimeter);
        updateBScanInterval();
    }
}

qreal ExamManager::getBScanDiameter(void) const
{
    return m_bscanDiameter;
}

void ExamManager::setBScanDiameter(qreal p_value)
{
    if( m_bscanDiameter != p_value ) {
        m_bscanDiameter = p_value;
        m_bscanPerimeter = M_PI*m_bscanDiameter;
        emit evBScanDiameterChanged(m_bscanDiameter);
        emit evBScanPerimeterChanged(m_bscanPerimeter);
        updateBScanInterval();
    }
}

quint32 ExamManager::getMepNbCol(void) const
{
    return m_mepNbCol;
}

void ExamManager::setMepNbCol(quint32 p_value)
{
    if( m_mepNbCol != p_value ) {
        m_mepNbCol = p_value;
        emit evMepNbColChanged(m_mepNbCol);
        updateMepIntervalX();
    }
}

qreal ExamManager::getMepPerimeter(void) const
{
    return m_mepPerimeter;
}

void ExamManager::setMepPerimeter(qreal p_value)
{
    if( m_mepPerimeter != p_value ) {
        m_mepPerimeter = p_value;
        m_mepDiameter = m_mepPerimeter/M_PI;
        emit evMepDiameterChanged(m_mepDiameter);
        emit evMepPerimeterChanged(m_mepPerimeter);
        updateMepIntervalX();
    }
}

qreal ExamManager::getMepDiameter(void) const
{
    return m_mepDiameter;
}

void ExamManager::setMepDiameter(qreal p_value)
{
    if( m_mepDiameter != p_value ) {
        m_mepDiameter = p_value;
        m_mepPerimeter = M_PI*m_mepDiameter;
        emit evMepDiameterChanged(m_mepDiameter);
        emit evMepPerimeterChanged(m_mepPerimeter);
        updateMepIntervalX();
    }
}

qreal ExamManager::getMepHeight(void) const
{
    return m_mepHeight;
}

void ExamManager::setMepHeight(qreal p_value)
{
    if( m_mepHeight != p_value ) {
        m_mepHeight = p_value;
        emit evMepHeightChanged(m_mepHeight);
        updateMepIntervalY();
    }
}

quint32 ExamManager::getMepNbRow(void) const
{
    return m_mepNbRow;
}

void ExamManager::setMepNbRow(quint32 p_value)
{
    if( m_mepNbRow != p_value ) {
        m_mepNbRow = p_value;
        emit evMepNbRowChanged(m_mepNbRow);
        updateMepIntervalY();
    }
}

qreal ExamManager::getMepIntervalX(void) const
{
    return m_mepIntervalX;
}

void ExamManager::setMepIntervalX(qreal p_value)
{
    if( m_mepIntervalX != p_value ) {
        m_mepIntervalX = p_value;
        emit evMepIntervalXChanged(m_mepIntervalX);
    }
}

qreal ExamManager::getMepIntervalY(void) const
{
    return m_mepIntervalY;
}

void ExamManager::setMepIntervalY(qreal p_value)
{
    if( m_mepIntervalY != p_value ) {
        m_mepIntervalY = p_value;
        emit evMepIntervalYChanged(m_mepIntervalY);
    }
}

QString ExamManager::getCurrentExamName() const
{
    return m_currentExamName;
}

void ExamManager::setCurrentBScanGeneratrice(quint32 p_value)
{
    if( p_value != m_currentBScanGeneratice ) {
        m_currentBScanGeneratice = p_value;
        emit evCurrentBScanGeneratriceChanged(m_currentBScanGeneratice);
        updateCurrentExamName();
    }
}

quint32 ExamManager::getCurrentBScanGeneratrice(void) const
{
    return m_currentBScanGeneratice;
}

// -- Settings
bool ExamManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_structName);
    bRet &= XML_READ_UNIQUE(QString, m_pieceName);
    bRet &= XML_READ_UNIQUE(QString, m_exportFolder);
    bRet &= XML_READ_UNIQUE(quint32, m_controlType);
    bRet &= XML_READ_UNIQUE(quint32, m_bscanNbGeneratrice);
    bRet &= XML_READ_UNIQUE(qreal, m_bscanPerimeter);
    bRet &= XML_READ_UNIQUE(qreal, m_bscanDiameter);
    bRet &= XML_READ_UNIQUE(quint32, m_mepNbCol);
    bRet &= XML_READ_UNIQUE(qreal, m_mepPerimeter);
    bRet &= XML_READ_UNIQUE(qreal, m_mepDiameter);
    bRet &= XML_READ_UNIQUE(qreal, m_mepHeight);
    bRet &= XML_READ_UNIQUE(quint32, m_mepNbRow);
    return bRet;
}

bool ExamManager::customLoadDefaultSettings()
{
    bool bRet = true;
    setStructName("");
    setPieceName("");
    setControlType(CONTROL_BSCAN_GENERATICE);
    QStringList f = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    setExportFolder(f.first());
    setBScanPerimeter(1);
    setBScanDiameter(1);
    setBScanNbGeneratrice(16);
    setBScanDiameter(50);
    setMepNbCol(10);
    setMepPerimeter(1);
    setMepDiameter(1);
    setMepHeight(30);
    setMepNbRow(10);

    return bRet;
}

bool ExamManager::customApplySettings()
{
    bool bRet = true;
    emit evStructNameChanged(m_structName);
    emit evPieceNameChanged(m_pieceName);
    emit evExportFolderChanged(m_exportFolder);
    emit evControlTypeChanged(m_controlType);
    emit evBScanNbGeneratriceChanged(m_bscanNbGeneratrice);
    emit evBScanPerimeterChanged(m_bscanPerimeter);
    emit evBScanDiameterChanged(m_bscanDiameter);
    emit evMepNbColChanged(m_mepNbCol);
    emit evMepPerimeterChanged(m_mepPerimeter);
    emit evMepDiameterChanged(m_mepDiameter);
    emit evMepHeightChanged(m_mepHeight);
    emit evMepNbRowChanged(m_mepNbRow);

    updateMepIntervalX();
    updateMepIntervalY();
    updateBScanInterval();
    updateCurrentExamName();
    return bRet;
}

bool ExamManager::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_structName);
    bRet &= XML_WRITE_UNIQUE(m_pieceName);
    bRet &= XML_WRITE_UNIQUE(m_exportFolder);
    bRet &= XML_WRITE_UNIQUE(m_controlType);
    bRet &= XML_WRITE_UNIQUE(m_bscanNbGeneratrice);
    bRet &= XML_WRITE_UNIQUE(m_bscanInterval);
    bRet &= XML_WRITE_UNIQUE(m_bscanPerimeter);
    bRet &= XML_WRITE_UNIQUE(m_bscanDiameter);
    bRet &= XML_WRITE_UNIQUE(m_mepNbCol);
    bRet &= XML_WRITE_UNIQUE(m_mepPerimeter);
    bRet &= XML_WRITE_UNIQUE(m_mepDiameter);
    bRet &= XML_WRITE_UNIQUE(m_mepHeight);
    bRet &= XML_WRITE_UNIQUE(m_mepNbRow);
    return bRet;
}

void ExamManager::updateBScanInterval()
{
    // Compute new interval :
    setBScanInterval(m_bscanPerimeter/m_bscanNbGeneratrice);
}

void ExamManager::updateMepIntervalX()
{
    setMepIntervalX(m_mepPerimeter/m_mepNbCol);
}

void ExamManager::updateMepIntervalY()
{
    setMepIntervalY(m_mepHeight/m_mepNbRow);
}

void ExamManager::updateCurrentExamName()
{
    QString newName = m_structName + "\n" + m_pieceName;
    if( CONTROL_BSCAN_GENERATICE == m_controlType ) {
        if( !m_pieceName.isEmpty() ) {
            newName += " "; // add a space
        }
        newName += QString("#%1").arg(m_currentBScanGeneratice, 2, 10, QLatin1Char('0'));
    }
    if( m_currentExamName != newName ) {
        m_currentExamName = newName;
        emit evCurrentExamNameChanged(m_currentExamName);
    }
}



