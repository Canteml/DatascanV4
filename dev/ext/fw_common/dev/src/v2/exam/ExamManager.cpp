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
#include <v2/exam/ExamManager.h>
#include <v2/scan/AScan.h>
#include <v2/scan/Surface1d.h>
#include <v2/scan/Location1d.h>
#include <v2/exam/ExportCsv.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorFuncPosition.h>
#include <v2/scan/MepMatrix.h>

// -- Qt Includes
#include <QStandardPaths>
#include <QtMath>
#include <QFileInfo>
#include <QDir>

ExamManager* ExamManager::m_pMyInstance = nullptr;

ExamManager::ExamManager() : XmlSettingsLoggable("ExamManager")
{
    m_bscanInterval = 0;
    m_mepIntervalX = 0;
    m_mepIntervalY = 0;
    m_bscanNbGeneratrix = 1;
    m_selectedBScanGeneratrix = 0;
    m_pSelectedLocation1d = nullptr;
    m_pMepMatrix = nullptr;
    m_currentMepRow = 0;
    m_currentMepColumn = 0;
    m_waitForMepAScan = false;
    m_isMepAutoMode = false;
    m_measureDisplay = MEASURE_ME;
    m_examStep = EXAM_STOPPED;
    m_examWriteProgress = 0;
    m_mepZoom = 1.0;

    m_pExportCsv = new ExportCsv();
    connect(m_pExportCsv, &ExportCsv::evProgessChanged, this, &ExamManager::onExportProgressChanged, Qt::QueuedConnection);
    connect(m_pExportCsv, &ExportCsv::evExportTerminated, this, &ExamManager::onExportTerminatedChanged, Qt::QueuedConnection);
}

ExamManager::~ExamManager()
{
    m_pMyInstance = nullptr;

    // unselect location
    m_pSelectedLocation1d = nullptr;
    m_selectedBScanGeneratrix = 0;
    emit evRequestSurfaceUpdate(nullptr);

    // delete all locations
    if( !m_lLocation1d.isEmpty() ) {
        for(auto i : qAsConst(m_lLocation1d)) {
            i->setRecord(false);
            i->clearScan();
            delete i;
        }
        m_lLocation1d.clear();
    }

    delete m_pMepMatrix;
    m_pMepMatrix = nullptr;

    m_pExportCsv->cancelExport();
    delete m_pExportCsv;
    m_pExportCsv = nullptr;
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

QStringList ExamManager::getCommonPieceNames() const
{
    return m_commonPieceNames;
}

void ExamManager::setPieceName(const QString& p_value)
{
    if( m_pieceName != p_value ) {
        m_pieceName = p_value;
        updateCurrentExamName();
        emit evPieceNameChanged(m_pieceName);
        // put the last used piece name at the top
        m_commonPieceNames.removeAll(p_value);
        m_commonPieceNames.push_front(p_value);
        while(m_commonPieceNames.size() > 5) {
            m_commonPieceNames.removeLast();
        }
        emit evCommonPieceNamesChanged(m_commonPieceNames);
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

QDateTime ExamManager::getMepBegin()
{
    return m_mepBegin;
}

QString ExamManager::getSiteName() const
{
    return m_site;
}

void ExamManager::setSiteName(const QString& p_value)
{
    if(p_value != m_site) {
        m_site = p_value;
        emit evSiteNameChanged(m_site);
    }
}

QString ExamManager::getSectorName() const
{
    return m_sector;
}

void ExamManager::setSectorName(const QString& p_value)
{
    if(p_value != m_sector) {
        m_sector = p_value;
        emit evSectorNameChanged(m_sector);
    }
}

bool ExamManager::isStructureActive() const
{
    return m_bStructIsActive;
}

void ExamManager::setStructureActive(const bool p_value)
{
    if(p_value != m_bStructIsActive) {
        m_bStructIsActive = p_value;
        emit evStructureActiveStatusChanged(m_bStructIsActive);
    }
}

QString ExamManager::getExamComment() const
{
    return m_examComment;
}

void ExamManager::setExamComment(const QString& p_comment)
{
    if(p_comment != m_examComment) {
        m_examComment = p_comment;
        emit evExamCommentChanged(m_examComment);
    }
}

QString ExamManager::getCellsAnnotations() const
{
    return m_cellsAnnotations;
}

void ExamManager::setCellsAnnotations(const QString& p_annotations)
{
    if(p_annotations != m_cellsAnnotations) {
        m_cellsAnnotations = p_annotations;
        emit evCellsAnnotationsChanged(m_cellsAnnotations);
    }
}

bool ExamManager::getMepVirolesEnabled() const
{
    return m_bMepVirolesEnabled;
}

void ExamManager::setMepVirolesEnabled(const bool p_bEnabled)
{
    if(p_bEnabled != m_bMepVirolesEnabled) {
        m_bMepVirolesEnabled = p_bEnabled;
        emit evMepVirolesEnabledChanged(m_bMepVirolesEnabled);
    }
}

quint32 ExamManager::getMeasuresPerVirole() const
{
    return m_u32MeasuresPerVirole;
}

void ExamManager::setMeasuresPerVirole(const quint32 p_count)
{
    if(p_count == 0) {
        setMeasuresPerVirole(1);
        return;
    }
    if(p_count != m_u32MeasuresPerVirole && p_count > 0) {
        m_u32MeasuresPerVirole = p_count;
        emit evMeasuresPerViroleChanged(m_u32MeasuresPerVirole);
    }
}

QString ExamManager::getExamOrigin() const
{
    return m_examOrigin;
}

void ExamManager::setExamOrigin(const QString& p_value)
{
    if(p_value != m_examOrigin) {
        m_examOrigin = p_value;
        emit evExamOriginChanged(m_examOrigin);
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
        initControl();
    }
}

quint32 ExamManager::getBScanNbGeneratrix(void) const
{
    return m_bscanNbGeneratrix;
}

void ExamManager::setBScanNbGeneratrix(quint32 p_value)
{
    if( m_bscanNbGeneratrix != p_value && p_value > 0) {
        m_bscanNbGeneratrix = p_value;
        emit evBScanNbGeneratrixChanged(m_bscanNbGeneratrix);
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
        resetMepMatrix();
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
        resetMepMatrix();
        emit evMepNbRowChanged(m_mepNbRow);
        updateMepIntervalY();
    }
}

void ExamManager::addMepRowBefore()
{
    m_pMepMatrix->addRow(m_currentMepRow);
    m_mepNbRow += 1;
    setMepRow(m_currentMepRow + 1);
    setMepHeight(getMepHeight() + getMepIntervalY());
    emit evMepNbRowChanged(m_mepNbRow);
}

void ExamManager::addMepRowAfter()
{
    m_pMepMatrix->addRow(m_currentMepRow + 1);
    m_mepNbRow += 1;
    setMepHeight(getMepHeight() + getMepIntervalY());
    emit evMepNbRowChanged(m_mepNbRow);
}

void ExamManager::addMepColBefore()
{
    m_pMepMatrix->addColumn(m_currentMepColumn);
    m_mepNbCol += 1;
    setMepCol(m_currentMepColumn + 1);
    emit evMepNbColChanged(m_mepNbCol);
}

void ExamManager::addMepColAfter()
{
    m_pMepMatrix->addColumn(m_currentMepColumn + 1);
    m_mepNbCol += 1;
    emit evMepNbColChanged(m_mepNbCol);
}

void ExamManager::removeCurrentMepRow()
{
    if(m_mepNbRow == 1) {
        return;
    }
    m_pMepMatrix->removeRow(m_currentMepRow);
    m_mepNbRow -= 1;
    if(m_currentMepRow >= m_mepNbRow) {
        setMepRow(m_currentMepRow - 1);
    }
    m_mepHeight -= getMepIntervalY();
    emit evMepHeightChanged(m_mepHeight);
    emit evMepNbRowChanged(m_mepNbRow);
}

void ExamManager::removeCurrentMepCol()
{
    if(m_mepNbCol == 1) {
        return;
    }
    m_pMepMatrix->removeColumn(m_currentMepColumn);
    m_mepNbCol -= 1;
    if(m_currentMepColumn >= m_mepNbCol) {
        setMepCol(m_currentMepColumn - 1);
    }
    emit evMepNbColChanged(m_mepNbRow);
}

quint32 ExamManager::getMepCol() const
{
    return m_currentMepColumn;
}

quint32 ExamManager::getMepRow() const
{
    return m_currentMepRow;
}

void ExamManager::setMepCol(quint32 p_u32Col)
{
    if(p_u32Col < 0 || p_u32Col >= m_mepNbCol) {
        return;
    }
    if(p_u32Col != m_currentMepColumn) {
        m_currentMepColumn = p_u32Col;
        emit evMepColChanged(m_currentMepColumn);
    }
}

void ExamManager::setMepRow(quint32 p_u32Row)
{
    if(p_u32Row < 0 || p_u32Row >= m_mepNbRow) {
        return;
    }
    if(p_u32Row != m_currentMepRow) {
        m_currentMepRow = p_u32Row;
        emit evMepRowChanged(m_currentMepRow);
    }
}

void ExamManager::resetMepMatrix()
{
    if(m_pMepMatrix != nullptr) {
        m_waitForMepAScan = false;
        m_pMepMatrix->resetSize({ m_mepNbRow, m_mepNbCol});
        setMepRow(0);
        setMepCol(0);
    }
}

bool ExamManager::getZoneAuto(void) const
{
    return m_zoneAuto;
}

void ExamManager::setZoneAuto(bool p_value)
{
    if( p_value != m_zoneAuto ) {
        m_zoneAuto = p_value;
        emit evZoneAutoChanged(m_zoneAuto);
    }
}

qreal ExamManager::getZoneAutoHoleSizeMm()
{
    return m_zoneAutoHoleSizeMm;
}

void ExamManager::setZoneAutoHoleSizeMm(qreal p_value)
{
    if( p_value != m_zoneAutoHoleSizeMm ) {
        m_zoneAutoHoleSizeMm = p_value;
        emit evZoneAutoHoleSizeMmChanged(m_zoneAutoHoleSizeMm);
    }
}

void ExamManager::zoneAddToCurrentPosition()
{
    if( nullptr != m_pSelectedLocation1d ) {
        m_pSelectedLocation1d->zoneAddToCurrentPosition();
    }
}

void ExamManager::zoneFixBounds()
{
    if( nullptr != m_pSelectedLocation1d ) {
        m_pSelectedLocation1d->zoneFixBounds();
    }
}

void ExamManager::zoneRemoveLast()
{
    if( nullptr != m_pSelectedLocation1d ) {
        m_pSelectedLocation1d->zoneRemoveLast();
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
        emit evMepHeaderUpdated();
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
        /*if(m_mepIntervalY != m_mepHeight/(m_mepNbRow - 1)) {
            m_mepNbRow = (int) ceil((m_mepHeight / m_mepIntervalY) + 1);
            m_mepIntervalY = m_mepHeight/(m_mepNbRow - 1);
        }
        emit evMepNbRowChanged(m_mepNbRow);*/
        emit evMepIntervalYChanged(m_mepIntervalY);
        emit evMepHeaderUpdated();
    }
}

QString ExamManager::getCurrentExamName() const
{
    return m_currentExamName;
}

Location1d *ExamManager::getSelectedLocation() const
{
    return m_pSelectedLocation1d;
}

qint32 ExamManager::getExamStep() const
{
    return m_examStep;
}

void ExamManager::setExamStep(qint32 p_value)
{
    if( m_examStep != p_value ) {
        qint32 oldExamStep = m_examStep;
        m_examStep = p_value;

        if( EXAM_STOPPED == oldExamStep ) {
            if( CONTROL_BSCAN_GENERATRIX == m_controlType ) {
                examStartBScan();
            }
        }

        if( EXAM_EXPORTING == m_examStep ) {
            if( CONTROL_BSCAN_GENERATRIX == m_controlType ) {
                Surface1d * s0 = m_pSelectedLocation1d->getSurface(0);
                QVector<qreal> * p0 = s0->getArrayPositionM();
                // something to export ?
                if( p0->isEmpty() ) {
                    m_examStep = EXAM_STOPPED;
                } else {
                    // close the zones
                    m_pSelectedLocation1d->zoneFixBounds();
                    // GUI will dispaly new menu to end the exam
                }
            }
        }

        emit evExamStepChanged(m_examStep);
    }
}

bool ExamManager::isRecording() const
{
    return (EXAM_STOPPED == m_examStep || EXAM_RECORDING == m_examStep );
}

qint32 ExamManager::getExamWriteProgress() const
{
    return m_examWriteProgress;
}

void ExamManager::setExamWriteProgress(qint32 p_value)
{
    m_examWriteProgress = p_value;
    emit evExamWriteProgressChanged(m_examWriteProgress);
}

QString ExamManager::makeExamFullFileName(const QString &p_file) const
{
    QString fullFileName = QString("%1/%2").arg(m_exportFolder).arg(p_file);
    if( fullFileName.right(4) != ".csv" ) {
        fullFileName.append(".csv");
    }
    return fullFileName;
}

void ExamManager::startExamWrite(const QString &p_file)
{
    // reset progress
    setExamWriteProgress(0);
    // make filename
    QString fullFileName = makeExamFullFileName(p_file);
    // start write
    m_pExportCsv->startExport(fullFileName);
    setExamStep(EXAM_EXPORTING);
    fLogInfo(QString("Export CSV : %1").arg(fullFileName));
}

void ExamManager::cancelExamWrite()
{
    m_pExportCsv->cancelExport();
}

QString ExamManager::getExamGeneratedFileName() const
{
    QString sRet;
    if( CONTROL_BSCAN_GENERATRIX == m_controlType ) {
        sRet = QString("%1-%2#%3").arg(m_structName).arg(m_pieceName).arg(m_selectedBScanGeneratrix+1, 2, 10, QChar('0'));
    }
    return sRet;
}

qint32 ExamManager::checkFileName(const QString &p_file) const
{
    /////< return 0 if npo error, 1 if directory is invalid, 2 if the file already exists
    qint32 iRet = 0;
    // check path
    if( !QDir(m_exportFolder).exists() ) {
        iRet = 1;
    } else {
        // Check if the file name is not already used :
        QString fullFileName = makeExamFullFileName(p_file);
        if( QFileInfo::exists(fullFileName) ) {
            iRet = 2;
        }
    }
    return iRet;
}

QString ExamManager::getExportFullFileName() const
{
    return m_pExportCsv->getFileName();
}

// --

void ExamManager::pushAScan(AScan *p_pAscan)
{
    // try to attribut this AScan to a location
    if( nullptr != m_pSelectedLocation1d ) {    // BSCAN
        // For now, we only manage 1 BScan (for source 0)
        if( 0 == p_pAscan->m_source ) {
            m_pSelectedLocation1d->pushAScan(p_pAscan);
        } else {
            delete p_pAscan;
        }
    } else if( nullptr != m_pMepMatrix ) {      // MEP
        if( 0 == p_pAscan->m_source && m_waitForMepAScan ) {
            m_pMepMatrix->pushAScan(p_pAscan, m_currentMepRow, m_currentMepColumn);
            onDirectionChanged(VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLVectorDirX());
            m_waitForMepAScan = false;
            if(false == getIsMepAutoMode()) {
                goToNextMepCell();
            }
        } else {
            delete p_pAscan;
        }
    } else {
        // delete ASCAN if no Location or matrix selected
        delete p_pAscan;
    }
}

quint32 ExamManager::getSelectedBScanGeneratrix() const
{
    return m_selectedBScanGeneratrix;
}

void ExamManager::setSelectedBScanGeneratrix(quint32 p_value)
{
    if( p_value != m_selectedBScanGeneratrix || nullptr == m_pSelectedLocation1d) {
        if( p_value >= (quint32)m_lLocation1d.size() ) {
            fLogError(QString("bad index : %1 >= %2").arg(p_value).arg(m_lLocation1d.size() ));
        } else {
// FOR NOW, do not change location, juste reset current ...
//            // unselect last location
//            if( nullptr != m_pSelectedLocation1d ) {
//                m_pSelectedLocation1d->setRecord(false);
//            }
//            m_selectedBScanGeneratrix = p_value;
//            m_pSelectedLocation1d = m_lLocation1d.at(p_value);
//            m_pSelectedLocation1d->setRecord(true);
            // --
            m_selectedBScanGeneratrix = p_value;
            selectCurrentGeneratrix();
            updateCurrentExamName();
            emit evSelectedBScanGeneratrixChanged(m_selectedBScanGeneratrix);
        }
    }
}

void ExamManager::selectNextGeneratrix()
{
    setSelectedBScanGeneratrix(getGeneratrixNumberNext());
}

void ExamManager::selectPreviousGeneratrix()
{
    setSelectedBScanGeneratrix(getGeneratrixNumberPrevious());
}

void ExamManager::selectCurrentGeneratrix()
{
    m_pSelectedLocation1d=m_lLocation1d.at(m_selectedBScanGeneratrix);
    m_pSelectedLocation1d->setRecord(true);
    m_pSelectedLocation1d->clearScan();
}

qint32 ExamManager::getGeneratrixNumberPrevious() const
{
    qint32 g = m_selectedBScanGeneratrix - 1;
    if( g < 0 ) {
        g = m_bscanNbGeneratrix - 1;
    }
    return g;
}

qint32 ExamManager::getGeneratrixNumberNext() const
{
    qint32 g = m_selectedBScanGeneratrix + 1;
    if( g >= (qint32)m_bscanNbGeneratrix ) {
        g = 0;
    }
    return g;
}

Location1d *ExamManager::getCurrentBScanGeneratrix()
{
    return m_lLocation1d.at(m_selectedBScanGeneratrix);
}

MepMatrix *ExamManager::getMepMatrix()
{
    return m_pMepMatrix;
}

quint32 ExamManager::getMeasureDisplay() const
{
    return m_measureDisplay;
}

void ExamManager::setMeasureDisplay(quint32 p_value)
{
    if( m_measureDisplay != p_value ) {
        m_measureDisplay = p_value;
        emit evMeasureDisplayChanged(m_measureDisplay);
    }
}

// -- Settings
bool ExamManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_structName, "-");
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_pieceName, "-");
    if( ! XML_READ_UNIQUE(QString, m_exportFolder) ) {
            QStringList f = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
            setExportFolder(f.first());
    }
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_controlType, CONTROL_BSCAN_GENERATRIX);
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_site, "");
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_sector, "");
    bRet &= XML_READ_UNIQUE_DEFAULT(bool, m_bStructIsActive, false);
    bRet &= XML_READ_UNIQUE_DEFAULT(bool, m_bMepVirolesEnabled, false);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_u32MeasuresPerVirole, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_examOrigin, "");
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_bscanNbGeneratrix, 16);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_bscanPerimeter, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_bscanDiameter, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_mepNbCol, 10);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_mepPerimeter, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_mepDiameter, 50);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_mepHeight, 30);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_mepNbRow, 10);
    QString slCommonPieceNames;
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, slCommonPieceNames, "");
    m_commonPieceNames = slCommonPieceNames.split(",");
    bRet &= XML_READ_UNIQUE_DEFAULT(bool, m_zoneAuto,  true);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_zoneAutoHoleSizeMm, 30);
    return bRet;
}

bool ExamManager::customLoadDefaultSettings()
{
    bool bRet = true;
    setStructName("");
    setPieceName("");
    setControlType(CONTROL_BSCAN_GENERATRIX);
    setSiteName("");
    setSectorName("");
    setStructureActive(false);
    setMepVirolesEnabled(false);
    setMeasuresPerVirole(1);
    setExamOrigin("");
    QStringList f = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    setExportFolder(f.first());
    setBScanPerimeter(1);
    setBScanDiameter(1);
    setBScanNbGeneratrix(16);
    setBScanDiameter(50);
    setMepNbCol(10);
    setMepPerimeter(1);
    setMepDiameter(1);
    setMepHeight(30);
    setMepNbRow(10);
    setZoneAuto(true);
    setZoneAutoHoleSizeMm(30);

    return bRet;
}

bool ExamManager::customApplySettings()
{
    bool bRet = true;
    emit evStructNameChanged(m_structName);
    emit evPieceNameChanged(m_pieceName);
    emit evExportFolderChanged(m_exportFolder);
    emit evControlTypeChanged(m_controlType);
    emit evSiteNameChanged(m_site);
    emit evSectorNameChanged(m_sector);
    emit evStructureActiveStatusChanged(m_bStructIsActive);
    emit evMepVirolesEnabledChanged(m_bMepVirolesEnabled);
    emit evMeasuresPerViroleChanged(m_u32MeasuresPerVirole);
    emit evExamOriginChanged(m_examOrigin);
    emit evBScanNbGeneratrixChanged(m_bscanNbGeneratrix);
    emit evBScanPerimeterChanged(m_bscanPerimeter);
    emit evBScanDiameterChanged(m_bscanDiameter);
    emit evMepNbColChanged(m_mepNbCol);
    emit evMepPerimeterChanged(m_mepPerimeter);
    emit evMepDiameterChanged(m_mepDiameter);
    emit evMepHeightChanged(m_mepHeight);
    emit evMepNbRowChanged(m_mepNbRow);
    emit evCommonPieceNamesChanged(m_commonPieceNames);
    emit evZoneAutoChanged(m_zoneAuto);
    emit evZoneAutoHoleSizeMmChanged(m_zoneAutoHoleSizeMm);

    updateMepIntervalX();
    updateMepIntervalY();
    updateBScanInterval();
    updateCurrentExamName();
    initControl();

    return bRet;
}

bool ExamManager::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_structName);
    bRet &= XML_WRITE_UNIQUE(m_pieceName);
    bRet &= XML_WRITE_UNIQUE(m_exportFolder);
    bRet &= XML_WRITE_UNIQUE(m_controlType);
    bRet &= XML_WRITE_UNIQUE(m_site);
    bRet &= XML_WRITE_UNIQUE(m_sector);
    bRet &= XML_WRITE_UNIQUE(m_bStructIsActive);
    bRet &= XML_WRITE_UNIQUE(m_examComment);
    bRet &= XML_WRITE_UNIQUE(m_cellsAnnotations);
    bRet &= XML_WRITE_UNIQUE(m_bMepVirolesEnabled);
    bRet &= XML_WRITE_UNIQUE(m_u32MeasuresPerVirole);
    bRet &= XML_WRITE_UNIQUE(m_examOrigin);
    bRet &= XML_WRITE_UNIQUE(m_bscanNbGeneratrix);
    bRet &= XML_WRITE_UNIQUE(m_bscanInterval);
    bRet &= XML_WRITE_UNIQUE(m_bscanPerimeter);
    bRet &= XML_WRITE_UNIQUE(m_bscanDiameter);
    bRet &= XML_WRITE_UNIQUE(m_mepNbCol);
    bRet &= XML_WRITE_UNIQUE(m_mepPerimeter);
    bRet &= XML_WRITE_UNIQUE(m_mepDiameter);
    bRet &= XML_WRITE_UNIQUE(m_mepHeight);
    bRet &= XML_WRITE_UNIQUE(m_mepNbRow);

    QString slCommonPieceNames;
    for(int i=0; i<m_commonPieceNames.size(); i++) {
        slCommonPieceNames+=m_commonPieceNames.at(i);
        if( i<m_commonPieceNames.size()-1 ) {
            slCommonPieceNames.append(",");
        }
    }
    bRet &= XML_WRITE_UNIQUE(slCommonPieceNames);
    bRet &= XML_WRITE_UNIQUE(m_zoneAuto);
    bRet &= XML_WRITE_UNIQUE(m_zoneAutoHoleSizeMm);
    return bRet;
}

void ExamManager::updateBScanInterval()
{
    // Compute new interval :
    setBScanInterval(m_bscanPerimeter/m_bscanNbGeneratrix);
}

void ExamManager::updateMepIntervalX()
{
    setMepIntervalX(m_mepPerimeter/m_mepNbCol);
}

void ExamManager::updateMepIntervalY()
{
    if(m_mepNbRow == 1) {
        setMepIntervalY(m_mepHeight);
        return;
    }
    setMepIntervalY(m_mepHeight/(m_mepNbRow - 1));
}

void ExamManager::updateCurrentExamName()
{
    QString newName = m_structName + "\n" + m_pieceName;
    if( CONTROL_BSCAN_GENERATRIX == m_controlType ) {
        if( !m_pieceName.isEmpty() ) {
            newName += " "; // add a space
        }
        newName += QString("#%1").arg(m_selectedBScanGeneratrix+1, 2, 10, QLatin1Char('0'));
    }
    if( m_currentExamName != newName ) {
        m_currentExamName = newName;
        emit evCurrentExamNameChanged(m_currentExamName);
    }
}

void ExamManager::initControl()
{
    // unselect location
    m_pSelectedLocation1d = nullptr;
    m_selectedBScanGeneratrix = 0;
    emit evRequestSurfaceUpdate(nullptr);

    // delete all locations
    if( !m_lLocation1d.isEmpty() ) {
        for(auto i : qAsConst(m_lLocation1d)) {
            i->setRecord(false);
            i->clearScan();
            delete i;
        }
        m_lLocation1d.clear();
    }

    delete m_pMepMatrix;
    m_pMepMatrix = nullptr;
    m_waitForMepAScan = false;
    setIsMepAutoMode(false);
    m_mepZoom = 1.0;

    m_currentMepRow = 0;
    m_currentMepColumn = 0;

    setExamComment("");
    setCellsAnnotations("");

    if( CONTROL_BSCAN_GENERATRIX == m_controlType ) {
        for(quint32  i=0; i<m_bscanNbGeneratrix; i++) {
            m_lLocation1d.append(new Location1d(QString::number(i)));
            connect(m_lLocation1d.last(), &Location1d::evRequestSurfaceUpdate, this, &ExamManager::onRequestSurfaceUpdate);
            connect(m_lLocation1d.last(), &Location1d::evLocationZoneChanged, this, &ExamManager::onLocationZoneChanged, Qt::QueuedConnection);
        }
        setSelectedBScanGeneratrix(0);
    }

    if( CONTROL_BSCAN_MEP == m_controlType ) {
        m_pMepMatrix = new MepMatrix();
        resetMepMatrix();
        connect(m_pMepMatrix, &MepMatrix::evDataUpdated, this, &ExamManager::onMepDataChanged);
        m_mepBegin = QDateTime::currentDateTime();
    }

    emit evRequestLocationZoneUpdate(m_pSelectedLocation1d);
    emit evMepUpdated(m_pMepMatrix);
    emit evMepHeaderUpdated();
}

void ExamManager::onRequestSurfaceUpdate(Surface1d *p_surface)
{
    // emit signal for BSCAN
    emit evRequestSurfaceUpdate(p_surface);
}

void ExamManager::onLocationZoneChanged(Location1d * p_location)
{
    if( p_location == m_pSelectedLocation1d ) {
        emit evRequestLocationZoneUpdate(p_location);
    }
}

void ExamManager::onExportProgressChanged(qint32 p_value)
{
    setExamWriteProgress(p_value);
}

void ExamManager::onExportTerminatedChanged(bool p_success)
{
    emit evExportTerminatedChanged(p_success);
}

void ExamManager::startWaitingForMepAScan()
{
    m_waitForMepAScan = true;
}

void ExamManager::goToNextMepCell()
{
    qint8 i8Direction = VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLVectorDirX(); // 0 = up ; 1 = down
    qint8 i8ToAdd = i8Direction * 2 - 1; // map 0 to -1 and 1 to 1
    if( (( m_currentMepRow == m_mepNbRow - 1 ) && (i8ToAdd > 0)) ||
        (( m_currentMepRow == 0 ) && ( i8ToAdd < 0 )) ) {
        if( m_currentMepColumn == m_mepNbCol - 1 ) {
            return;
        }
        setMepCol(m_currentMepColumn + 1);
        setIsMepAutoMode(false);
        VectorManager::getInstance()->getVectorStd()->funcPosition()->setParamLVectorDirX( 1 - i8Direction );
        return;
    }
    setMepRow(m_currentMepRow + i8ToAdd);
}

void ExamManager::removeMepAScan(quint32 p_u32Row, quint32 p_u32Column)
{
    if(nullptr != m_pMepMatrix) {
        m_pMepMatrix->pushAScan(nullptr, p_u32Row, p_u32Column);
    }
}

bool ExamManager::getIsMepAutoMode()
{
    return m_isMepAutoMode;
}

void ExamManager::setIsMepAutoMode(bool p_bIsAutomatic)
{
    if(m_isMepAutoMode != p_bIsAutomatic) {
        m_isMepAutoMode = p_bIsAutomatic;
        VectorFuncPosition *funcPos = VectorManager::getInstance()->getVectorStd()->funcPosition();
        if(m_isMepAutoMode) {
            qreal currentPos = ( (m_mepNbRow - 1) - m_currentMepRow ) * m_mepIntervalY /*+ m_mepIntervalY/2.0*/;
            //currentPos += funcPos->getParamLVectorDirX() * m_mepIntervalY;
            funcPos->setParamLBscanPosMmX(currentPos * 1000);
            m_waitForMepAScan = true;
            connect(funcPos, &VectorFuncPosition::paramLBscanPosMmXChanged, this, &ExamManager::onNewPosition);
        } else {
            m_waitForMepAScan = false;
            disconnect(funcPos, &VectorFuncPosition::paramLBscanPosMmXChanged, this, &ExamManager::onNewPosition);
        }
        emit evMepAutoModeChanged(m_isMepAutoMode);
    }
}

void ExamManager::onNewPosition(qreal rMmXPosition)
{
    qreal rMXPosition = rMmXPosition / 1000;
    if( (m_controlType != CONTROL_BSCAN_MEP) || (false == m_isMepAutoMode) ) {
        return;
    }
    quint8 u8Dir = VectorManager::getInstance()->getVectorStd()->funcPosition()->getParamLVectorDirX();
    quint32 u32Row = m_mepNbRow - u8Dir - (rMXPosition / m_mepIntervalY);
    if(u32Row != getMepRow()) {
        setMepRow(u32Row);
        m_waitForMepAScan = true;
        if( (u8Dir == 1 && u32Row == m_mepNbRow - 1) || (u8Dir == 0 && u32Row == 0) ) {
            setIsMepAutoMode(false);
            m_waitForMepAScan = true;
        }
    }
}

void ExamManager::onDirectionChanged(quint8 p_u8NewDir)
{
    if(CONTROL_BSCAN_MEP != m_controlType) {
        return;
    }
    m_pMepMatrix->setColumnDirection(m_currentMepColumn, p_u8NewDir);
    emit evMepHeaderUpdated();
}

void ExamManager::examStartBScan()
{
    // reset position
    VectorManager::getInstance()->getVectorStd()->funcPosition()->setParamLBscanPosMmX(0);
    // remove pending ASCAN
    VectorManager::getInstance()->getVectorStd()->clearAScan();
    // clear BSCAN
    m_pSelectedLocation1d->clearScan();
}

void ExamManager::onMepDataChanged()
{
    emit evMepUpdated(m_pMepMatrix);
}

qreal ExamManager::getMepZoom()
{
    return m_mepZoom;
}

void ExamManager::setMepZoom(qreal p_rZoom)
{
    if(p_rZoom < 0.1) {
        return;
    }
    if(p_rZoom != m_mepZoom) {
        m_mepZoom = p_rZoom;
        emit evMepZoomChanged(m_mepZoom);
    }
}



