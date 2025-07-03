/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamManager.h
# Author      : Thierry Lechevallier
# Date        : 02/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
class Location1d;
class AScan;
class Surface1d;
class ExportCsv;
class MepMatrix;

// -- Qt Includes
#include <QObject>
#include <QTime>

/**
 * @brief The ExamManager class
 *
 * In a first version, we will store here all data required to generate a CSV for old Datareport.
 * Much more data will be added in the futur, provided by a model of data thinkfor Datareport usages
 *
 */
class ExamManager : public QObject, public XmlSettingsLoggable
{
    Q_OBJECT
    // Settings
    Q_PROPERTY(QString structName READ getStructName WRITE setStructName NOTIFY evStructNameChanged)
    Q_PROPERTY(QString pieceName READ getPieceName WRITE setPieceName NOTIFY evPieceNameChanged)
    Q_PROPERTY(QStringList commonPieceNames READ getCommonPieceNames NOTIFY evCommonPieceNamesChanged)
    Q_PROPERTY(QString exportFolder READ getExportFolder WRITE setExportFolder NOTIFY evExportFolderChanged)
    Q_PROPERTY(quint32 controlType READ getControlType WRITE setControlType NOTIFY evControlTypeChanged)
    Q_PROPERTY(quint32 bscanNbGeneratrix READ getBScanNbGeneratrix WRITE setBScanNbGeneratrix NOTIFY evBScanNbGeneratrixChanged)
    Q_PROPERTY(qreal bscanPerimeter READ getBScanPerimeter WRITE setBScanPerimeter NOTIFY evBScanPerimeterChanged)
    Q_PROPERTY(qreal bscanDiameter READ getBScanDiameter WRITE setBScanDiameter NOTIFY evBScanDiameterChanged)
    Q_PROPERTY(quint32 mepNbCol READ getMepNbCol WRITE setMepNbCol NOTIFY evMepNbColChanged)
    Q_PROPERTY(qreal mepPerimeter READ getMepPerimeter WRITE setMepPerimeter NOTIFY evMepPerimeterChanged)
    Q_PROPERTY(qreal mepDiameter READ getMepDiameter WRITE setMepDiameter NOTIFY evMepDiameterChanged)
    Q_PROPERTY(qreal mepHeight READ getMepHeight WRITE setMepHeight NOTIFY evMepHeightChanged)
    Q_PROPERTY(quint32 mepNbRow READ getMepNbRow WRITE setMepNbRow NOTIFY evMepNbRowChanged)
    Q_PROPERTY(QString siteName READ getSiteName WRITE setSiteName NOTIFY evSiteNameChanged)
    Q_PROPERTY(QString sectorName READ getSectorName WRITE setSectorName NOTIFY evSectorNameChanged)
    Q_PROPERTY(bool structureActive READ isStructureActive WRITE setStructureActive NOTIFY evStructureActiveStatusChanged)
    Q_PROPERTY(QString examComment READ getExamComment WRITE setExamComment NOTIFY evExamCommentChanged)
    Q_PROPERTY(QString cellsAnnotations READ getCellsAnnotations WRITE setCellsAnnotations NOTIFY evCellsAnnotationsChanged)
    Q_PROPERTY(bool virolesEnabled READ getMepVirolesEnabled WRITE setMepVirolesEnabled NOTIFY evMepVirolesEnabledChanged)
    Q_PROPERTY(quint32 measuresPerVirole READ getMeasuresPerVirole WRITE setMeasuresPerVirole NOTIFY evMeasuresPerViroleChanged)
    Q_PROPERTY(QString examOrigin READ getExamOrigin WRITE setExamOrigin NOTIFY evExamOriginChanged)

    Q_PROPERTY(bool zoneAuto READ getZoneAuto WRITE setZoneAuto NOTIFY evZoneAutoChanged)
    Q_PROPERTY(qreal zoneAutoHoleSizeMm READ getZoneAutoHoleSizeMm WRITE setZoneAutoHoleSizeMm NOTIFY evZoneAutoHoleSizeMmChanged)


    // Behaviour
    Q_PROPERTY(qreal bscanInterval READ getBScanInterval WRITE setBScanInterval NOTIFY evBScanIntervalChanged)
    Q_PROPERTY(qreal mepIntervalX READ getMepIntervalX WRITE setMepIntervalX NOTIFY evMepIntervalXChanged)
    Q_PROPERTY(qreal mepIntervalY READ getMepIntervalY WRITE setMepIntervalY NOTIFY evMepIntervalYChanged)

    Q_PROPERTY(qreal mepCol READ getMepCol WRITE setMepCol NOTIFY evMepColChanged);
    Q_PROPERTY(qreal mepRow READ getMepRow WRITE setMepRow NOTIFY evMepRowChanged);
    Q_PROPERTY(bool isMepAutoMode READ getIsMepAutoMode WRITE setIsMepAutoMode NOTIFY evMepAutoModeChanged)

    Q_PROPERTY(QString currentExamName READ getCurrentExamName NOTIFY evCurrentExamNameChanged)

    Q_PROPERTY(qint32 examStep READ getExamStep WRITE setExamStep NOTIFY evExamStepChanged)
    Q_PROPERTY(qint32 examWriteProgress READ getExamWriteProgress WRITE setExamWriteProgress NOTIFY evExamWriteProgressChanged)

    // Execution
    Q_PROPERTY(quint32 selectedBScanGeneratrix READ getSelectedBScanGeneratrix WRITE setSelectedBScanGeneratrix NOTIFY evSelectedBScanGeneratrixChanged)
    Q_PROPERTY(quint32 MeasureDisplay READ getMeasureDisplay WRITE setMeasureDisplay NOTIFY evMeasureDisplayChanged)

    Q_PROPERTY(qreal mepZoom READ getMepZoom WRITE setMepZoom NOTIFY evMepZoomChanged)


public :

    /**
     * @brief The CONTOL_TYPE enum
     *
     * Type of Controls that can be done with Datascan
     */
    enum CONTOL_TYPE {
        CONTROL_BSCAN_GENERATRIX,
        CONTROL_BSCAN_MEP,
        CONTROL_PIPE
    };

    enum MEASURE_DISPLAY {
        MEASURE_1E,    ///< 1 Echo
        MEASURE_ME   ///< Multiple Echos
    };

    enum EXAM_STEP {
        EXAM_STOPPED = 0,   ///< Measure are displayed in BSCAN/MEP ... but will be reseted when going to RECORDING
        EXAM_RECORDING = 1, ///< Measure are stored function to vector position
        EXAM_PAUSED = 2,    ///< Measure are not displayed / stored in BSCAN/MEP
        EXAM_EXPORTING = 3  ///< Stored measures are writed in a file. No new measures stored. Go to stopped step after.
    };

    ~ExamManager();

    static ExamManager* makeInstance();
    static ExamManager* getInstance();
    void initManager();

    // -- Properties
    // Settings
    QString getStructName(void) const;
    void setStructName(const QString& p_value);
    QString getPieceName(void) const;
    QStringList getCommonPieceNames(void) const;
    void setPieceName(const QString& p_value);
    QString getExportFolder(void) const;
    void setExportFolder(const QString& p_value);
    QDateTime getMepBegin();

    QString getSiteName() const;
    void setSiteName(const QString& p_value);
    QString getSectorName() const;
    void setSectorName(const QString& p_value);
    bool isStructureActive() const;
    void setStructureActive(const bool p_value);
    QString getExamComment() const;
    void setExamComment(const QString& p_comment);
    QString getCellsAnnotations() const;
    void setCellsAnnotations(const QString& p_annotations);
    bool getMepVirolesEnabled() const;
    void setMepVirolesEnabled(const bool p_bEnabled);
    quint32 getMeasuresPerVirole() const;
    void setMeasuresPerVirole(const quint32 p_count);
    QString getExamOrigin() const;
    void setExamOrigin(const QString& p_value);

    quint32 getControlType(void) const;
    void setControlType(quint32 p_value);
    quint32 getBScanNbGeneratrix(void) const;
    void setBScanNbGeneratrix(quint32 p_value);
    qreal getBScanPerimeter(void) const;
    void setBScanPerimeter(qreal p_value);
    qreal getBScanDiameter(void) const;
    void setBScanDiameter(qreal p_value);

    quint32 getMepNbCol(void) const;
    void setMepNbCol(quint32 p_value);

    qreal getMepPerimeter(void) const;
    void setMepPerimeter(qreal p_value);
    qreal getMepDiameter(void) const;
    void setMepDiameter(qreal p_value);

    qreal getMepHeight(void) const;
    void setMepHeight(qreal p_value);
    quint32 getMepNbRow(void) const;
    void setMepNbRow(quint32 p_value);

    Q_INVOKABLE void addMepRowBefore();
    Q_INVOKABLE void addMepRowAfter();
    Q_INVOKABLE void addMepColBefore();
    Q_INVOKABLE void addMepColAfter();
    Q_INVOKABLE void removeCurrentMepRow();
    Q_INVOKABLE void removeCurrentMepCol();

    quint32 getMepCol() const;
    quint32 getMepRow() const;
    void setMepCol(quint32 p_u32Col);
    void setMepRow(quint32 p_u32Col);

    void resetMepMatrix();

    bool getZoneAuto(void) const;
    void setZoneAuto(bool p_value);
    qreal getZoneAutoHoleSizeMm();
    void setZoneAutoHoleSizeMm(qreal p_value);

    // Zones
    Q_INVOKABLE void zoneAddToCurrentPosition();
    Q_INVOKABLE void zoneFixBounds();
    Q_INVOKABLE void zoneRemoveLast();

    // -- Behaviour

    qreal getBScanInterval(void) const;
    void setBScanInterval(qreal p_value);
    qreal getMepIntervalX(void) const;
    void setMepIntervalX(qreal p_value);
    qreal getMepIntervalY(void) const;
    void setMepIntervalY(qreal p_value);

    QString getCurrentExamName() const;

    Location1d* getSelectedLocation() const;

    // Exam
    qint32 getExamStep() const;
    void setExamStep(qint32 p_value);
    bool isRecording() const; ///< true in steps EXAM_STOPPED and EXAM_RECORDING

    qint32 getExamWriteProgress() const;
    void setExamWriteProgress(qint32 p_value);

    Q_INVOKABLE QString makeExamFullFileName(const QString &p_file) const;
    Q_INVOKABLE void startExamWrite(const QString &p_file);
    Q_INVOKABLE void cancelExamWrite();
    Q_INVOKABLE QString getExamGeneratedFileName() const;   ///< return file name function to structure, piece ...
    Q_INVOKABLE QString getExportFullFileName() const;        ///< return Path+File name
    Q_INVOKABLE qint32 checkFileName(const QString &p_file) const;        ///< return 0 if npo error, 1 if directory is invalid, 2 if the file already exists

    // ASCAN
    void pushAScan(AScan* p_pAscan);

    // Execution
    quint32 getSelectedBScanGeneratrix() const;
    void setSelectedBScanGeneratrix(quint32 p_value);
    Q_INVOKABLE void selectNextGeneratrix();
    Q_INVOKABLE void selectPreviousGeneratrix();
    Q_INVOKABLE void selectCurrentGeneratrix();
    Q_INVOKABLE qint32 getGeneratrixNumberPrevious() const;
    Q_INVOKABLE qint32 getGeneratrixNumberNext() const;
    Location1d* getCurrentBScanGeneratrix();

    MepMatrix* getMepMatrix();
    Q_INVOKABLE void startWaitingForMepAScan();
    Q_INVOKABLE void goToNextMepCell();
    Q_INVOKABLE void removeMepAScan(quint32 p_u32Row, quint32 p_u32Column);

    bool getIsMepAutoMode();
    void setIsMepAutoMode(bool p_bIsAutomatic);

    quint32 getMeasureDisplay() const;
    void setMeasureDisplay(quint32 p_value);

    qreal getMepZoom();
    void setMepZoom(qreal p_rZoom);

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    void updateBScanInterval();
    void updateMepIntervalX();
    void updateMepIntervalY();
    void updateCurrentExamName();

    void initControl();

signals :
    void evStructNameChanged(QString);
    void evPieceNameChanged(QString);
    void evCommonPieceNamesChanged(QStringList);
    void evExportFolderChanged(QString);
    void evControlTypeChanged(quint32);
    void evBScanNbGeneratrixChanged(quint32);
    void evBScanIntervalChanged(qreal);
    void evBScanPerimeterChanged(qreal);
    void evBScanDiameterChanged(qreal);

    void evSiteNameChanged(QString);
    void evSectorNameChanged(QString);
    void evStructureActiveStatusChanged(bool);
    void evExamCommentChanged(QString);
    void evCellsAnnotationsChanged(QString);
    void evMepVirolesEnabledChanged(bool);
    void evMeasuresPerViroleChanged(quint32);
    void evExamOriginChanged(QString);

    void evMepNbColChanged(quint32);
    void evMepPerimeterChanged(qreal);
    void evMepDiameterChanged(qreal);
    void evMepNbRowChanged(quint32);
    void evMepHeightChanged(qreal);
    void evMepIntervalXChanged(qreal);
    void evMepIntervalYChanged(qreal);

    void evMepColChanged(quint32);
    void evMepRowChanged(quint32);
    void evMepAutoModeChanged(bool);

    void evZoneAutoChanged(bool);
    void evZoneAutoHoleSizeMmChanged(qreal);

    void evCurrentExamNameChanged(QString);
    void evSelectedBScanGeneratrixChanged(quint32);

    void evMeasureDisplayChanged(quint32);

    void evRequestSurfaceUpdate(Surface1d*);
    void evRequestLocationZoneUpdate(Location1d*);

    void evExamStepChanged(qint32 p_value);
    void evExamWriteProgressChanged(bool p_success);
    void evExportTerminatedChanged(bool);

    void evMepUpdated(MepMatrix*);
    void evMepHeaderUpdated();
    void evMepZoomChanged(qreal);

private slots :
    void onRequestSurfaceUpdate(Surface1d*);
    void onLocationZoneChanged(Location1d*);

    void onExportProgressChanged(qint32);
    void onExportTerminatedChanged(bool);

    void onMepDataChanged();
    void onNewPosition(qreal rMmXPosition);
    void onDirectionChanged(quint8 p_u8NewDir);

private :
    void examStartBScan();

private :
    ExamManager();

    // -- Behaviour
    static ExamManager* m_pMyInstance;

    // -- Properties / settings
    QString m_structName;
    QString m_pieceName;
    QDateTime m_mepBegin;
    QStringList m_commonPieceNames;  ///< last 10 pieces used
    QString m_exportFolder;         ///< where files are exported after an exam
    quint32 m_controlType;          ///< value in CONTOL_TYPE
    quint32 m_bscanNbGeneratrix;
    qreal m_bscanPerimeter;
    qreal m_bscanDiameter;
    quint32 m_mepNbCol;
    qreal m_mepPerimeter;
    qreal m_mepDiameter;
    quint32 m_mepNbRow;
    qreal m_mepHeight;

    QString m_site;
    QString m_sector;
    bool m_bStructIsActive;
    QString m_examComment;
    QString m_cellsAnnotations;
    bool m_bMepVirolesEnabled;
    quint32 m_u32MeasuresPerVirole;
    QString m_examOrigin;

    bool m_zoneAuto;            ///< try to create a new zone when a virole is detected
    qreal m_zoneAutoHoleSizeMm; ///< size of the hole use to detect new virole (default is 30 mm)


    // -- Behaviour
    qreal m_bscanInterval;
    qreal m_mepIntervalX;
    qreal m_mepIntervalY;

    QString m_currentExamName;

    QList<Location1d*> m_lLocation1d;   ///< used for BSCANs
    Location1d* m_pSelectedLocation1d;  ///< current Location in use if BASCAN is selected. Set to nullptr if no Location1d selected.

    MepMatrix *m_pMepMatrix;
    bool m_waitForMepAScan;
    bool m_isMepAutoMode;

    qint32 m_examStep;              ///< value in EXAM_STEP, manage state chart of recording
    qint32 m_examWriteProgress;     ///< signal to GUI the progress while writing stored measures. Value : 0..100% / -1 = error

    ExportCsv * m_pExportCsv;

    // -- Execution
    quint32 m_selectedBScanGeneratrix;      ///<  current Location in use if BASCAN is selected. Set to nullptr if no Location1d selected.
    quint32 m_measureDisplay;   ///< value in MEASURE_DISPLAY (1 echo / multi echo)

    quint32 m_currentMepRow;
    quint32 m_currentMepColumn;
    qreal m_mepZoom;
};

