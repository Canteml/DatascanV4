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

//    Q_PROPERTY(QString examCurrentMeasure0 READ getExamCurrentMeasure0 NOTIFY examCurrentMeasure0Changed)
//    Q_PROPERTY(QString examCurrentMeasure1 READ getExamCurrentMeasure1 NOTIFY examCurrentMeasure1Changed)
//    Q_PROPERTY(QString examCurrentMeasure2 READ getExamCurrentMeasure2 NOTIFY examCurrentMeasure2Changed)
//    Q_PROPERTY(QString examCurrentMeasure3 READ getExamCurrentMeasure3 NOTIFY examCurrentMeasure3Changed)
//    Q_PROPERTY(QString examCurrentPicture0 READ getExamCurrentPicture0 NOTIFY examCurrentPicture0Changed)
//    Q_PROPERTY(QString examCurrentPicture1 READ getExamCurrentPicture1 NOTIFY examCurrentPicture1Changed)
//    Q_PROPERTY(QString examCurrentPicture2 READ getExamCurrentPicture2 NOTIFY examCurrentPicture2Changed)
//    Q_PROPERTY(QString examCurrentPicture3 READ getExamCurrentPicture3 NOTIFY examCurrentPicture3Changed)

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

//    QString getExamCurrentMeasure0() const;
//    QString getExamCurrentMeasure1() const;
//    QString getExamCurrentMeasure2() const;
//    QString getExamCurrentMeasure3() const;
//    QString getExamCurrentPicture0() const;
//    QString getExamCurrentPicture1() const;
//    QString getExamCurrentPicture2() const;
//    QString getExamCurrentPicture3() const;

    // -- Behaviour
    Q_INVOKABLE void nextAcquisition(); ///< go to the next acuiqition
    Q_INVOKABLE void resetExam();       ///< clear current exam, clear qml form
    Q_INVOKABLE void exportExam(QString& p_fileName);   ///< export exam to excel file

    Q_INVOKABLE void takeMeasure();     ///< for the currentSensorNumber, save the thickness measure
    Q_INVOKABLE void takePicture();       ///< for the currentSensorNumber, capture a photo

private :
    void setAcquisitionNumber(qint32 p_value);
    QString getExamCurrentMeasure(qint32 p_sensorNumber) const;
    void clearCircular();

private slots :
    void onMainCameraCaptureSaved(QString p_fileName);

signals :
    // -- Properties
    void acquisitionNumberChanged(qint32 p_value);
    void examCurrentCommentChanged(QString p_value);
    void currentSensorNumberChanged(qint32 p_value);

//    void examCurrentMeasure0Changed(QString p_value);
//    void examCurrentMeasure1Changed(QString p_value);
//    void examCurrentMeasure2Changed(QString p_value);
//    void examCurrentMeasure3Changed(QString p_value);
//    void examCurrentPicture0Changed(QString p_value);
//    void examCurrentPicture1Changed(QString p_value);
//    void examCurrentPicture2Changed(QString p_value);
//    void examCurrentPicture3Changed(QString p_value);

    // -- Event
    void evPrepareNextAcquisition();
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

    // -- Manager
    CameraManager* m_pCameraManager;
    ScanManager* m_pScanManager;
    ExamManager * m_pExamManager;

};
