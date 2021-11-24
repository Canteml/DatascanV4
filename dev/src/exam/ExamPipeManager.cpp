/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamPipeManager.cpp
# Author      : Thierry Lechevallier
# Date        : 17/11/2021
#
# ==========================================================================*/

// -- Internal includes
#include "ExamPipeManager.h"
#include "camera/CameraManager.h"
#include "scan/ScanManager.h"
#include <v2/exam/ExamManager.h>
#include <coreUi/CoreUiQml.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>

// -- Qt includes
#include <QFileInfo>


// -- QXlsx includes
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "xlsxcell.h"

ExamPipeManager* ExamPipeManager::m_pMyInstance = nullptr;


ExamPipeManager::ExamPipeManager() : Loggable("ExamPipeManager")
{
    m_pExamPipe = new ExamPipe;
    m_acquisitionNumber = 0;
    m_currentSensorNumber = 0;
    m_examModified = false;
    m_pExamPipe->m_lExamCircular.append(new ExamCircular());
    m_pCurrentCircular = m_pExamPipe->m_lExamCircular.last();
}

ExamPipeManager::~ExamPipeManager()
{
    delete m_pExamPipe;
    m_pExamPipe = nullptr;
}

ExamPipeManager * ExamPipeManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("ExamPipeManager::makeInstance instance already exists !");
    } else {
        m_pMyInstance = new ExamPipeManager();
    }

    return m_pMyInstance;
}

ExamPipeManager * ExamPipeManager::getInstance()
{
    return m_pMyInstance;
}

void ExamPipeManager::init()
{
    m_pCameraManager = CameraManager::getInstance();
    m_pScanManager = ScanManager::getInstance();
    m_pExamManager = ExamManager::getInstance();

    connect(m_pCameraManager, &CameraManager::evMainCameraCaptureSaved, this, &ExamPipeManager::onMainCameraCaptureSaved);
    connect(m_pExamManager, &ExamManager::evControlTypeChanged, this, &ExamPipeManager::onExamControlTypeChanged);
    connect(VectorManager::getInstance()->getVectorStd(), &VectorStd::prodIdChanged, this, &ExamPipeManager::onVectorProductIdChanged);
}

// -- Properties
qint32 ExamPipeManager::getAcquisitionNumber() const
{
    return m_acquisitionNumber;
}

void ExamPipeManager::setAcquisitionNumber(qint32 p_value)
{
    m_acquisitionNumber = p_value;
    emit acquisitionNumberChanged(m_acquisitionNumber);
}

QString ExamPipeManager::getExamCurrentMeasure(qint32 p_sensorNumber) const
{
    QString sRet = "";
    qreal m =  m_pCurrentCircular->m_aExamPipeSensor[p_sensorNumber].m_measureMm;
    if( -1 != m ) {
        sRet = QString("%1 mm").arg(m, 0, 'F', 1, QChar('0'));
    }
    return sRet;
}

void ExamPipeManager::clearCircular()
{
    emit examCurrentCommentChanged("");
    emit evClearCircular();
}

void ExamPipeManager::setExamModified(bool p_value)
{
    if( m_examModified != p_value ) {
        m_examModified = p_value;
        if( m_examModified ) {
            m_startExamTime = QDateTime::currentDateTime();
        }
    }
}

void ExamPipeManager::onMainCameraCaptureSaved(QString p_fileName)
{
    if( m_pExamManager->getControlType() == ExamManager::CONTROL_PIPE ) {
        emit evSensorPictureChanged(m_currentSensorNumber, p_fileName);
    }
}

void ExamPipeManager::onExamControlTypeChanged(quint32 p_type)
{
    if( ExamManager::CONTROL_PIPE == p_type) {
        resetExam();
    }
}

void ExamPipeManager::onVectorProductIdChanged(qint32)
{
    if( m_pCameraManager->getMainCameraSelectAuto() ) {
        setCurrentSensorNumber(0);
    }
}

QString ExamPipeManager::getExamCurrentComment() const
{
    return m_pCurrentCircular->m_comment;
}

void ExamPipeManager::setExamCurrentComment(const QString & p_value)
{
    m_pCurrentCircular->m_comment = p_value;
    emit examCurrentCommentChanged(m_pCurrentCircular->m_comment);
}

qint32 ExamPipeManager::getCurrentSensorNumber() const
{
    return m_currentSensorNumber;
}

void ExamPipeManager::setCurrentSensorNumber(qint32 p_value)
{
    m_currentSensorNumber = p_value;
    m_pCameraManager->setRoleMainCamera(CameraRole::ROLE_SENSOR1+m_currentSensorNumber);
    // For now, with Panda V3, we have only 3 sensors, the number 3 is used also to take measure of sensor 4
    m_pScanManager->setSelectedAScanIndex(qMin(m_currentSensorNumber,2));
    emit currentSensorNumberChanged(m_currentSensorNumber);
}

// -- Behaviour

void ExamPipeManager::nextAcquisition()
{
    m_pExamPipe->m_lExamCircular.append(new ExamCircular());
    m_pCurrentCircular = m_pExamPipe->m_lExamCircular.last();
    setAcquisitionNumber(m_acquisitionNumber + 1);

    clearCircular();
    setCurrentSensorNumber(0);
}

void ExamPipeManager::resetExam()
{
    m_pExamPipe->clear();
    m_pExamPipe->m_lExamCircular.append(new ExamCircular());
    m_pCurrentCircular = m_pExamPipe->m_lExamCircular.last();
    setAcquisitionNumber(0);
    setCurrentSensorNumber(0);
    setExamModified(false);

    clearCircular();
}

void ExamPipeManager::takeMeasure()
{
    setExamModified(true);
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_measureMm = m_pScanManager->getAScanThicknessMmMultiEcho();
    emit evSensorThickessChanged(m_currentSensorNumber, getExamCurrentMeasure(m_currentSensorNumber));
}

void ExamPipeManager::takePicture()
{
    setExamModified(true);
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_photoFileName = m_pCameraManager->captureMainCamera(QString("Acq %1 - ").arg(m_acquisitionNumber));
}

bool ExamPipeManager::isExamModified()
{
    return m_examModified;
}

QString ExamPipeManager::exportExam(const QString& p_fileName)
{
    // fill excel template with data of exam
    QString sRet;
    // Find template function to the language
    QString fileTemplate = QString("%1/templates/pipe_fr.xlsx").arg(CoreUiQml::getDirAppRes());

    if( !QFileInfo::exists(fileTemplate) ) {
        fLogError("Template file not found : " + fileTemplate);
        sRet = tr("Base de données corrompue.");
    } else {
        QXlsx::Document doc(fileTemplate);
        if( !doc.load() ) {
            fLogError("Fail to open template : " + fileTemplate);
            sRet = tr("Erreur à l'ouverture de la base de données.");
        } else {
            // -- General Info
            doc.write("C8", m_startExamTime.toString("dd.MM.yyyy hh:mm:ss"));
            doc.write("C13", m_pExamManager->getStructName());
            doc.write("I13", m_pExamManager->getPieceName());

            qreal colWidth6 = 0;
            for(int i=2; i<8; i++) {
                colWidth6 += doc.columnWidth(i);
            }
            fLogDebug1(QString("ColWidth = %1").arg(colWidth6));


            // -- Vector Info
            doc.write("E17", VectorManager::getInstance()->getVectorStd()->getFuncGeneralParam().m_paramDProdUid);

            // -- Exam
            int yCircular = 58;
            // Pase all circular
            for(auto i : qAsConst(m_pExamPipe->m_lExamCircular) ) {
                // parse the 4 sensors
                for(int s = 0; s<4; s++) {
                    // thikness measure
                    if( ExamPipeSensor::SENSOR_NO_MEARURE != i->m_aExamPipeSensor[s].m_measureMm ) {
                        doc.write(yCircular+1, 2+s*6, QString("%1 mm").arg(((qreal)((qint32)(i->m_aExamPipeSensor[s].m_measureMm*10)))/10));
                    }
                    // photo
                    if( !i->m_aExamPipeSensor[s].m_photoFileName.isEmpty() ) {
                        // Image camera : W : 640 , H = 480
                        // Screen Excel H : 490 => 365 => ratio : 0.75 (0.755 after manual adjustement)
                        // New image format : 640 => 477 / 480 => 358
                        // CF31 integration : images are too bigs : H = 390 instead of 358
                        // New ratio computed with Paint : 0.59
                        qreal ratio_xls = 0.59; // 0.755
                        doc.insertImage(yCircular+1, 1+s*6, QImage(QString(i->m_aExamPipeSensor[s].m_photoFileName).replace("\\\\", "/")).scaled(640*ratio_xls, 480*ratio_xls));
                    }
                }
                // Comment
                doc.write(yCircular+1, 26, i->m_comment);
                // prepare next
                yCircular+=2; // One circular = 2 row
            }

            // -- Write file :
            QString fileName = p_fileName.right(p_fileName.length()-8); // remove "file:///" appenened by qml
            if( ".xlsx" != fileName.right(5) ) {
                fileName.append(".xlsx");
            }
            if( ! doc.saveAs(fileName) ) {
                fLogError("Fail to save file : " + fileName);
                sRet = tr("Impossible d'écrire le fichier.");
            }
        }
    }

    // reset 'exam modified' if no error
    if( sRet.isEmpty() ) {
        setExamModified(false);
    }

    return sRet;
}
