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
#include <QMap>
#include <QStringList>
#include <QByteArray>
#include <QVariantList>

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
    // Equipment
    Q_PROPERTY(QString equipmentNom READ getEquipmentNom WRITE setEquipmentNom NOTIFY evEquipmentNomChanged)
    Q_PROPERTY(QString equipmentNumero READ equipmentNumero WRITE setEquipmentNumero NOTIFY equipmentNumeroChanged)
    Q_PROPERTY(QString equipmentDate READ equipmentDate WRITE setEquipmentDate NOTIFY equipmentDateChanged)
    Q_PROPERTY(bool dateValide READ getDateValide NOTIFY evDateValideChanged)
    Q_PROPERTY(double equipmentFrequency READ equipmentFrequency WRITE setEquipmentFrequency NOTIFY equipmentFrequencyChanged)
    Q_PROPERTY(QString inspecteurNom READ inspecteurNom WRITE setInspecteurNom NOTIFY inspecteurNomChanged)
    Q_PROPERTY(QString inspecteurNumero READ inspecteurNumero WRITE setInspecteurNumero NOTIFY inspecteurNumeroChanged)
    Q_PROPERTY(QString inspecteurDate READ inspecteurDate WRITE setInspecteurDate NOTIFY inspecteurDateChanged)
    Q_PROPERTY(bool inspecteurValide READ inspecteurValide NOTIFY inspecteurValideChanged)
    Q_PROPERTY(QString supervisorName READ supervisorName WRITE setSupervisorName NOTIFY supervisorNameChanged)
    Q_PROPERTY(QString supervisorNumber READ supervisorNumber WRITE setSupervisorNumber NOTIFY supervisorNumberChanged)
    Q_PROPERTY(QString supervisorDate READ supervisorDate WRITE setSupervisorDate NOTIFY supervisorDateChanged)
    Q_PROPERTY(QString sondeNbVoies READ sondeNbVoies WRITE setSondeNbVoies NOTIFY sondeNbVoiesChanged)
    Q_PROPERTY(QString sondeNumero READ sondeNumero WRITE setSondeNumero NOTIFY sondeNumeroChanged)
    Q_PROPERTY(QString controlleurNom READ controlleurNom WRITE setControlleurNom NOTIFY controlleurNomChanged)
    Q_PROPERTY(QString sondeAnee READ sondeAnee WRITE setSondeAnee NOTIFY sondeAneeChanged)
    Q_PROPERTY(bool sondeDateValide READ getSondeDateValide NOTIFY evSondeDateValideChanged)
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

    // Planner import (TOML) : messages shown next to the import button
    Q_PROPERTY(QVariantList importMessages READ getImportMessages NOTIFY securityCheckChanged)
    Q_PROPERTY(bool importConforme READ isImportConforme NOTIFY securityCheckChanged)
    // Free session ("Session libre" in CentralPage0): no planner, always compliant
    Q_PROPERTY(bool freeSession READ isFreeSession WRITE setFreeSession NOTIFY evFreeSessionChanged)

    // User Session Control
    Q_PROPERTY(QString inputProfil READ inputProfil WRITE setInputProfil NOTIFY inputProfilChanged)
    Q_PROPERTY(QString inputUserName READ inputUserName WRITE setInputUserName NOTIFY inputUserNameChanged)
    Q_PROPERTY(QString inputUserPrenom READ inputUserPrenom WRITE setInputUserPrenom NOTIFY inputUserPrenomChanged)
    Q_PROPERTY(QString inputCarteNumero READ inputCarteNumero WRITE setInputCarteNumero NOTIFY inputCarteNumeroChanged)
    Q_PROPERTY(QString inputUSerPassword READ inputUSerPassword WRITE setInputUSerPassword NOTIFY inputUSerPasswordChanged)
    Q_PROPERTY(bool inputValidCheck READ inputValidCheck WRITE setInputValidCheck NOTIFY inputValidCheckChanged)


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
    Q_INVOKABLE bool importPlannerToml(const QString& p_filePath);
    QVariantList getImportMessages() const { return m_importMessages; }
    // -- false as soon as the last planner import reported a warning or an error
    bool isImportConforme() const;
    bool isFreeSession() const { return m_freeSession; }
    void setFreeSession(bool p_freeSession);

    // User Session Control
    QString inputProfil() const;
    void setInputProfil(const QString& p_value);
    QString inputUserName() const;
    void setInputUserName(const QString& p_value);
    QString inputUserPrenom() const;
    void setInputUserPrenom(const QString& p_value);
    QString inputCarteNumero() const;
    void setInputCarteNumero(const QString& p_value);
    QString inputUSerPassword() const;
    void setInputUSerPassword(const QString& p_value);
    bool inputValidCheck() const;
    void setInputValidCheck(bool value);

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

    Q_INVOKABLE void inputUserControl();

    // -- Planner import [equipment] block
    QString getEquipmentNom() const;
    void setEquipmentNom(const QString& p_value);
    QString equipmentNumero() const;
    void setEquipmentNumero(const QString& p_value);
    QString equipmentDate() const;
    void setEquipmentDate(const QString& p_value);
    bool getDateValide() const;
    double equipmentFrequency() const;
    void setEquipmentFrequency(double p_value);
    QString inspecteurNom() const;
    void setInspecteurNom(const QString& p_value);
    QString inspecteurNumero() const;
    void setInspecteurNumero(const QString& p_value);
    QString inspecteurDate() const;
    void setInspecteurDate(const QString& p_value);
    bool inspecteurValide() const;
    QString supervisorName() const;
    void setSupervisorName(const QString& p_value);
    QString supervisorNumber() const;
    void setSupervisorNumber(const QString& p_value);
    QString supervisorDate() const;
    void setSupervisorDate(const QString& p_value);
    QString sondeNbVoies() const;
    void setSondeNbVoies(const QString& p_value);
    QString sondeNumero() const;
    void setSondeNumero(const QString& p_value);
    QString controlleurNom() const;
    void setControlleurNom(const QString& p_value);
    QString sondeAnee() const;
    void setSondeAnee(const QString& p_value);
    bool getSondeDateValide() const;

    // -- Planner import (TOML) : [object] block
    void setObjectType(const QString& p_value);
    void setObjectProduit(const QString& p_value);
    void setObjectRobeDiameter(double p_value);
    void setObjectTuyauterieDiametreMm(double p_value);
    void setObjectTuyauterieDiametreIn(double p_value);
    void setObjectTuyauterieEpaisseur(double p_value);
    void setObjectTuyauterieLingueur(double p_value);
    void setObjectCoquePosition(const QString& p_value);
    void setObjectCoqueOrientation(const QString& p_value);
    void setObjectCoqueLongueur(double p_value);
    void setObjectFondDiametre(double p_value);
    void setObjectFondNbAnnulaires(int p_value);
    void setObjectFondNbCentrales(int p_value);
    void setObjectToitDiametre(double p_value);
    void setObjectToitEpNominale(double p_value);
    void setObjectToitType(const QString& p_value);

    // -- Planner import (TOML) : inspection parameters
    void setInspProductDensity(const QString& p_value);
    void setInspFillingHeight(const QString& p_value);
    void setInspSectionCount(const QString& p_value);
    void setInspViroleLength(int p_index, const QString& p_value);
    void setInspViroleThickness(int p_index, const QString& p_value);
    void setInspViroleSteelGrade(int p_index, const QString& p_value);

    // -- Planner import (TOML) : [Reperage] block
    void setMepVerticalDirection(const QString& p_value);
    void setMepHorizontalDirection(const QString& p_value);

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

    QMap<QString, QString> captureTrackedParams() const;

    void equipmentDateCheck(const QString& p_date);
    void inspecteurDateCheck(const QString& p_date);
    void sondeDateCheck(const QString& p_date);

signals :
    void evEquipmentNomChanged(QString);
    void equipmentNumeroChanged(QString);
    void equipmentDateChanged(QString);
    void  equipmentFrequencyChanged(double);
    void inspecteurNomChanged(QString);
    void inspecteurNumeroChanged(QString);
    void inspecteurDateChanged(QString);
    void supervisorNameChanged(QString);
    void supervisorNumberChanged(QString);
    void supervisorDateChanged(QString);
    void sondeNbVoiesChanged(QString);
    void sondeAneeChanged(QString);
    void evStructNameChanged(QString);
    void sondeNumeroChanged(QString);
    void controlleurNomChanged(QString);
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

    void securityCheckChanged();
    void evFreeSessionChanged();

    void evDateValideChanged(bool);
    void inspecteurValideChanged(bool);
    void evSondeDateValideChanged(bool);

    // User Session Control
    void inputProfilChanged();
    void inputUserNameChanged();
    void inputUserPrenomChanged();
    void inputCarteNumeroChanged();
    void inputUSerPasswordChanged();
    void inputValidCheckChanged();

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

    // -- Planner import: [equipment] block
    QString m_equipmentNom;
    QString m_equipmentNumero;
    QString m_equipmentDate;
    bool m_dateValide = true;
    double m_equipmentFrequency = 1000;
    QString m_inspecteurNom;
    QString m_inspecteurNumero;
    QString m_inspecteurDate;
    bool m_inspecteurValide = true;
    QString m_supervisorName;
    QString m_supervisorNumber;
    QString m_supervisorDate;
    QString m_sondeNbVoies;
    QString m_sondeAnee;
    bool m_sondeDateValide = true;
    QString m_sondeNumero;
    QString m_controlleurNom;

    // -- Planner import (TOML) : [object] block
    QString m_objectType;
    QString m_objectProduit;
    double m_objectRobeDiameter;
    double m_objectTuyauterieDiametreMm;
    double m_objectTuyauterieDiametreIn;
    double m_objectTuyauterieEpaisseur;
    double m_objectTuyauterieLingueur;
    QString m_objectCoquePosition;
    QString m_objectCoqueOrientation;
    double m_objectCoqueLongueur;
    double m_objectFondDiametre;
    int m_objectFondNbAnnulaires;
    int m_objectFondNbCentrales;
    double m_objectToitDiametre;
    double m_objectToitEpNominale;
    QString m_objectToitType;

    // -- Planner import (TOML) : inspection parameters
    QString m_inspDiameter;
    QString m_inspHeight;
    QString m_inspLength;
    QString m_inspProductDensity;
    QString m_inspFillingHeight;
    QString m_inspPressure;
    QString m_inspSectionCount;
    QString m_inspSteelQuality;
    QString m_inspOriginalThickness;
    QString m_inspGeneratricesCount;
    QString m_inspRaysCount;
    QString m_inspFirstGeneratricePos;
    QString m_inspAnnotations;
    QStringList m_inspViroleLengths;
    QStringList m_inspViroleThicknesses;
    QStringList m_inspViroleSteelGrades;

    // -- Planner import (TOML) : [Reperage] block
    QString m_mepVerticalDirection;
    QString m_mepHorizontalDirection;

    // -- Planner import (TOML) : security check results
    bool m_firstImport;
    bool m_securityPlannedMissing;
    bool m_securityMissingFile;
    bool m_securityModifiedFile;
    bool m_securityIncompatibleDate;
    bool m_securityNonReadable;
    QVariantList m_importMessages;
    bool m_freeSession = false;

    // -- Planner import (TOML) : key state, replayed into the manifest at export time
    bool m_importFileImported;
    bool m_importKeyMismatch;
    QByteArray m_importComputedKey;
    QByteArray m_importOriginalKey;
    QString m_importFileName;

    // -- Planner import (TOML) : baseline for [[modification]] tracking
    QMap<QString, QString> m_plannerSnapshot;
    bool m_plannerSnapshotValid;

    // User Session Control
    QString m_inputProfil;
    QString m_inputUserName;
    QString m_inputUserPrenom;
    QString m_inputCarteNumero;
    QString m_inputUSerPassword;
    bool m_inputValidCheck = false;
};

