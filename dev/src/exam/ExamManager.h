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

// -- Qt Includes
#include <QObject>

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

    // Behaviour
    Q_PROPERTY(qreal bscanInterval READ getBScanInterval WRITE setBScanInterval NOTIFY evBScanIntervalChanged)
    Q_PROPERTY(qreal mepIntervalX READ getMepIntervalX WRITE setMepIntervalX NOTIFY evMepIntervalXChanged)
    Q_PROPERTY(qreal mepIntervalY READ getMepIntervalY WRITE setMepIntervalY NOTIFY evMepIntervalYChanged)

    Q_PROPERTY(QString currentExamName READ getCurrentExamName NOTIFY evCurrentExamNameChanged)

    // Execution
    Q_PROPERTY(quint32 SelectedBScanGeneratrix READ getSelectedBScanGeneratrix WRITE setSelectedBScanGeneratrix NOTIFY evSelectedBScanGeneratrixChanged)

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

    // -- Behaviour

    qreal getBScanInterval(void) const;
    void setBScanInterval(qreal p_value);
    qreal getMepIntervalX(void) const;
    void setMepIntervalX(qreal p_value);
    qreal getMepIntervalY(void) const;
    void setMepIntervalY(qreal p_value);

    QString getCurrentExamName() const;

    // ASCAN
    void pushAScan(AScan* p_pAscan);

    // Execution
    quint32 getSelectedBScanGeneratrix() const;
    void setSelectedBScanGeneratrix(quint32 p_value);

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

    void evMepNbColChanged(quint32);
    void evMepPerimeterChanged(qreal);
    void evMepDiameterChanged(qreal);
    void evMepNbRowChanged(quint32);
    void evMepHeightChanged(qreal);
    void evMepIntervalXChanged(qreal);
    void evMepIntervalYChanged(qreal);

    void evCurrentExamNameChanged(QString);
    void evSelectedBScanGeneratrixChanged(quint32);

    void evRequestSurfaceUpdate(Surface1d*);

private slots :
    void onRequestSurfaceUpdate(Surface1d*);

private :
    ExamManager();

    // -- Behaviour
    static ExamManager* m_pMyInstance;

    // -- Properties / settings
    QString m_structName;
    QString m_pieceName;
    QStringList m_commonPieceNames;  ///< last 10 pieces used
    QString m_exportFolder;         ///< where files are exported after an exam
    quint32 m_controlType;          ///< value in
    quint32 m_bscanNbGeneratrix;
    qreal m_bscanPerimeter;
    qreal m_bscanDiameter;
    quint32 m_mepNbCol;
    qreal m_mepPerimeter;
    qreal m_mepDiameter;
    quint32 m_mepNbRow;
    qreal m_mepHeight;


    // -- Behaviour
    qreal m_bscanInterval;
    qreal m_mepIntervalX;
    qreal m_mepIntervalY;

    QString m_currentExamName;

    QList<Location1d*> m_lLocation1d;   ///< used for BSCANs
    Location1d* m_pSelectedLocation1d;  ///< current Location in use if BASCAN is selected. Set to nullptr if no Location1d selected.
    quint32 m_selectedBScanGeneratrix;      ///<  current Location in use if BASCAN is selected. Set to nullptr if no Location1d selected.

};

