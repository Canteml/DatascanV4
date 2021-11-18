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

// -- Qt includes
#include <QObject>

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

void ExamPipeManager::onMainCameraCaptureSaved(QString p_fileName)
{
    if( m_pExamManager->getControlType() == ExamManager::CONTROL_PIPE ) {
        emit evSensorPictureChanged(m_currentSensorNumber, p_fileName);
    }
}

void ExamPipeManager::onExamControlTypeChanged()
{
    resetExam();
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
    m_examModified = false;

    clearCircular();
}

void ExamPipeManager::exportExam(const QString& p_fileName)
{
    m_examModified = false;
    fLogDebug1("Save : " + p_fileName);
    // fill excel template with data of exam
}

void ExamPipeManager::takeMeasure()
{
    m_examModified = true;
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_measureMm = m_pScanManager->getAScanThicknessMmMultiEcho();
    emit evSensorThickessChanged(m_currentSensorNumber, getExamCurrentMeasure(m_currentSensorNumber));
}

void ExamPipeManager::takePicture()
{
    m_examModified = true;
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_photoFileName = m_pCameraManager->captureMainCamera(QString("Acq %1 - ").arg(m_acquisitionNumber));
}

bool ExamPipeManager::isExamModified()
{
    return m_examModified;
}
