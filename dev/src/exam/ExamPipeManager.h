/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamPipeManager.h
# Author      : Thierry Lechevallier
# Date        : 17/11/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include "ExamPipe.h"
class CameraManager;
class ScanManager;
class ExamManager;

// -- Qt includes
#include <QObject>
#include <QDateTime>

/**
 * @brief The ExamPipeManager class
 *
 * Exam for the Pipe structure
 * Need for Panva V3
 */

class ExamPipeManager : public QObject, public Loggable
{
    Q_OBJECT
    Q_PROPERTY(qint32 acquisitionNumber READ getAcquisitionNumber NOTIFY acquisitionNumberChanged)
    Q_PROPERTY(QString examCurrentComment READ getExamCurrentComment WRITE setExamCurrentComment NOTIFY examCurrentCommentChanged)
    Q_PROPERTY(qint32 currentSensorNumber READ getCurrentSensorNumber WRITE setCurrentSensorNumber NOTIFY currentSensorNumberChanged)

public:
    static ExamPipeManager * makeInstance();
    static ExamPipeManager * getInstance();

    void init();

    ~ExamPipeManager();

    // -- Properties
    qint32 getAcquisitionNumber() const;
    QString getExamCurrentComment() const;
    void setExamCurrentComment(const QString & p_value);
    qint32 getCurrentSensorNumber() const;
    void setCurrentSensorNumber(qint32 p_value);

    // -- Behaviour
    Q_INVOKABLE void nextAcquisition(); ///< go to the next acuiqition
    Q_INVOKABLE void resetExam();       ///< clear current exam, clear qml form
    Q_INVOKABLE QString exportExam(const QString &p_fileName);   ///< export exam to excel file

    Q_INVOKABLE void takeMeasure();     ///< for the currentSensorNumber, save the thickness measure
    Q_INVOKABLE void takePicture();       ///< for the currentSensorNumber, capture a photo

    Q_INVOKABLE bool isExamModified();  ///< return true if exam has been modified since last save

private :
    void setAcquisitionNumber(qint32 p_value);
    QString getExamCurrentMeasure(qint32 p_sensorNumber) const;
    void clearCircular();
    void setExamModified(bool p_value);

private slots :
    void onMainCameraCaptureSaved(QString p_fileName);
    void onExamControlTypeChanged(quint32 p_type);
    void onVectorProductIdChanged(qint32);

signals :
    // -- Properties
    void acquisitionNumberChanged(qint32 p_value);
    void examCurrentCommentChanged(QString p_value);
    void currentSensorNumberChanged(qint32 p_value);

    // -- Event
    void evSensorThickessChanged(qint32 p_sensorNumber, QString p_sensorThickness);
    void evSensorPictureChanged(qint32 p_sensorNumber, QString p_sensorPictureFile);
    void evClearCircular();

private:
    ExamPipeManager();
    static ExamPipeManager* m_pMyInstance;

    // -- Behaviour
    ExamPipe* m_pExamPipe;
    qint32 m_acquisitionNumber;
    qint32 m_currentSensorNumber;
    ExamCircular* m_pCurrentCircular;
    bool m_examModified;    ///< need to be saved
    QDateTime m_startExamTime;

    // -- Manager
    CameraManager* m_pCameraManager;
    ScanManager* m_pScanManager;
    ExamManager * m_pExamManager;
};
