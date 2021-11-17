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
    m_pCurrentCircular = nullptr;
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

    resetExam();
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
    m_pScanManager->setSelectedAScanIndex(qMin(m_currentSensorNumber,3));
    emit currentSensorNumberChanged(m_currentSensorNumber);
}

//QString ExamPipeManager::getExamCurrentMeasure0() const
//{
//    return getExamCurrentMeasure(0);
//}

//QString ExamPipeManager::getExamCurrentMeasure1() const
//{
//    return getExamCurrentMeasure(1);
//}

//QString ExamPipeManager::getExamCurrentMeasure2() const
//{
//    return getExamCurrentMeasure(2);
//}

//QString ExamPipeManager::getExamCurrentMeasure3() const
//{
//    return getExamCurrentMeasure(3);
//}

//QString ExamPipeManager::getExamCurrentPicture0() const
//{
//    return m_pCurrentCircular->m_aExamPipeSensor[0].m_photoFileName;
//}

//QString ExamPipeManager::getExamCurrentPicture1() const
//{
//    return m_pCurrentCircular->m_aExamPipeSensor[1].m_photoFileName;
//}

//QString ExamPipeManager::getExamCurrentPicture2() const
//{
//    return m_pCurrentCircular->m_aExamPipeSensor[2].m_photoFileName;
//}

//QString ExamPipeManager::getExamCurrentPicture3() const
//{
//    return m_pCurrentCircular->m_aExamPipeSensor[3].m_photoFileName;
//}

// -- Behaviour

void ExamPipeManager::nextAcquisition()
{
    m_pExamPipe->m_lExamCircular.append(new ExamCircular());
    m_pCurrentCircular = m_pExamPipe->m_lExamCircular.last();
    setAcquisitionNumber(m_acquisitionNumber + 1);

    clearCircular();
}

void ExamPipeManager::resetExam()
{
    m_pExamPipe->clear();
    m_pExamPipe->m_lExamCircular.append(new ExamCircular());
    m_pCurrentCircular = m_pExamPipe->m_lExamCircular.last();
    setAcquisitionNumber(0);
    setCurrentSensorNumber(0);

    clearCircular();
}

void ExamPipeManager::exportExam(QString& p_fileName)
{
    // fill excel template with data of exam
}

void ExamPipeManager::takeMeasure()
{
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_measureMm = m_pScanManager->getAScanThicknessMmMultiEcho();
    emit evSensorThickessChanged(m_currentSensorNumber, getExamCurrentMeasure(m_currentSensorNumber));
//    if( 0 == m_currentSensorNumber ) {
//        emit examCurrentMeasure0Changed(getExamCurrentMeasure0());
//    } else if( 1 == m_currentSensorNumber ) {
//        emit examCurrentMeasure1Changed(getExamCurrentMeasure1());
//    }else if( 2 == m_currentSensorNumber ) {
//        emit examCurrentMeasure2Changed(getExamCurrentMeasure2());
//    }else if( 3 == m_currentSensorNumber ) {
//        emit examCurrentMeasure3Changed(getExamCurrentMeasure3());
//    }
}

void ExamPipeManager::takePicture()
{
    m_pCurrentCircular->m_aExamPipeSensor[m_currentSensorNumber].m_photoFileName = m_pCameraManager->captureMainCamera(QString("Acq %1 - ").arg(m_acquisitionNumber));
    //emit evSensorPictureChanged(m_currentSensorNumber, m_pCurrentCircular->m_aExamPipeSensor[0].m_photoFileName);

//    if( 0 == m_currentSensorNumber ) {
//        emit examCurrentPicture0Changed(getExamCurrentPicture0());
//    } else if( 1 == m_currentSensorNumber ) {
//        emit examCurrentPicture1Changed(getExamCurrentPicture1());
//    }else if( 2 == m_currentSensorNumber ) {
//        emit examCurrentPicture2Changed(getExamCurrentPicture2());
//    }else if( 3 == m_currentSensorNumber ) {
//        emit examCurrentPicture3Changed(getExamCurrentPicture3());
//    }
}
