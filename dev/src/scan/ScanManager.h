/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AscanModel.h
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/scan/AScan.h>
#include <v2/vector/VectorFuncAScan.h>
#include <encoder/EncoderController.h>
class AScan;
class AScanGraph;
class BScanGraph;
class JkGate;
class ExamManager;


// -- Qt Includes
#include <QObject>
class QQmlApplicationEngine;
class QTimer;

class ScanManager : public QObject, public XmlSettingsLoggable
{
    Q_OBJECT

    // -- ASCAN
    Q_PROPERTY(quint32 SelectedAScanIndex READ getSelectedAScanIndex WRITE setSelectedAScanIndex NOTIFY evSelectedAScanIndexChanged)
    Q_PROPERTY(VectorFuncAScan* SelectedFuncAScan READ getSelectedFuncAScan NOTIFY evSelectedFuncAScanChanged)            

    // -- BSCAN
    Q_PROPERTY(quint32 BScanViewXMode READ getBScanViewXMode WRITE setBScanViewXMode NOTIFY evBScanViewXModeChanged)
    Q_PROPERTY(quint32 BScanViewYMode READ getBScanViewYMode WRITE setBScanViewYMode NOTIFY evBScanViewYModeChanged)
    Q_PROPERTY(qreal BScanAxeXScaleM READ getBScanAxeXScaleM WRITE setBScanAxeXScaleM NOTIFY evBScanAxeXScaleMChanged)
    Q_PROPERTY(qreal BScanAxeYMinMm READ getBScanAxeYMinMm WRITE setBScanAxeYMinMm NOTIFY evBScanAxeYMinMmChanged)
    Q_PROPERTY(qreal BScanAxeYMaxMm READ getBScanAxeYMaxMm WRITE setBScanAxeYMaxMm NOTIFY evBScanAxeYMaxMmChanged)

public :

    static ScanManager * makeInstance();
    static ScanManager * getInstance();

    ~ScanManager();    

    void init();
    void initQmlObject(QObject *p_pQmlTopLevel);
    void stop();

    quint32 getSelectedAScanIndex() const;
    void setSelectedAScanIndex(quint32 p_index);

    VectorFuncAScan* getSelectedFuncAScan() const;

    AScan* getCurrentAScan();

    // -- ASCAN

    Q_INVOKABLE void ascanZoomIn();
    Q_INVOKABLE void ascanZoomOut();

    Q_INVOKABLE qreal getAScanThicknessMmMultiEcho();
    Q_INVOKABLE qreal getAScanThicknessMm1Echo();

    Q_INVOKABLE void ascanSaveUserParams();
    Q_INVOKABLE void ascanLoadUserParams();
    Q_INVOKABLE bool ascanSavePreset(const QString& p_name);
    Q_INVOKABLE bool ascanPresetExists(const QString& p_name) const;
    Q_INVOKABLE void ascanResetDefaultParams();
    Q_INVOKABLE QStringList ascanListPresets() const;
    Q_INVOKABLE bool ascanLoadPreset(const QString& p_name);
    Q_INVOKABLE void bscanSaveUserParams();
    Q_INVOKABLE void bscanLoadUserParams();
    Q_INVOKABLE bool bscanSavePreset(const QString& p_name);
    Q_INVOKABLE bool bscanPresetExists(const QString& p_name) const;
    Q_INVOKABLE void bscanResetDefaultParams();
    Q_INVOKABLE QStringList bscanListPresets() const;
    Q_INVOKABLE bool bscanLoadPreset(const QString& p_name);

//    // -- BSCAN

    qint32 getBScanViewXMode(void) const;
    void setBScanViewXMode(quint32 p_value);

    qint32 getBScanViewYMode(void) const;
    void setBScanViewYMode(quint32 p_value);

    qreal getBScanAxeXScaleM(void) const;
    void setBScanAxeXScaleM(qreal p_value);

    qreal getBScanAxeYMinMm(void) const;
    void setBScanAxeYMinMm(qreal p_value);

    qreal getBScanAxeYMaxMm(void) const;
    void setBScanAxeYMaxMm(qreal p_value);

    void setEncoderController(EncoderController* p_pEncoder);

    static const qint32 ASCAN_MEASURE_REFRESH_DELAY = 100;


signals :
    void evSelectedAScanIndexChanged(quint32);
    void evSelectedFuncAScanChanged(VectorFuncAScan*);
    void evUpdateAScan();

    void evBScanViewXModeChanged(qint32);
    void evBScanViewYModeChanged(qint32);
    void evBScanAxeXScaleMChanged(qreal);
    void evBScanAxeYMinMmChanged(qreal);
    void evBScanAxeYMaxMmChanged(qreal);

    void evAScanPresetChanged();
    void evBScanPresetChanged();

private slots :
    void onNewAScanTreated(AScan*);
    void onVectorNbAScanEnabledChanged(qint32);
    void onAutoGainParamsChanged();
    void onMeasureGateChanged();

    void onPrivateBScanViewXModeChanged(qint32);
    void onPrivateBScanViewYModeChanged(qint32);
    void onPrivateBScanAxeXScaleMChanged(qreal);
    void onPrivateBScanAxeYMinMmChanged(qreal);
    void onPrivateBScanAxeYMaxMmChanged(qreal);

    void onExamManagerStepChanged(qint32);

    void onUpdateAScanTimed();

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    ScanManager();
    static ScanManager* m_pMyInstance;

    void onVectorProductChanged();    
    void setPrivateSelectedAScanIndex(quint32 p_index);
    void emitUpdateAScanTimed(bool p_force = false);
    void pushAutoGainParamsToGraph();
    void pushMeasureGateToGraph();
    bool bscanWriteParamsToFile(const QString& p_fileName);
    bool bscanReadParamsFromFile(const QString& p_fileName);

    // -- Behaviour
    quint32 m_selectedAScanIndex;
    QTimer * m_timUpdateAScan;
    qint64 m_lastUpdateAScan;

    JkGate * m_pGate0;
    JkGate * m_pGate1;
    JkGate * m_pGateNoise;

    // -- Manager
    AScan m_pAScan[VectorStd::NB_FUNC_ASCAN];
    AScanGraph* m_pAScanGraphMain;
    BScanGraph* m_pBScanGraphMain;
    VectorFuncAScan* m_pConnectedAutoGainAScan = nullptr;
    VectorStd * m_pVector;
    ExamManager * m_pExamManager;
    EncoderController * m_pEncoderController = nullptr;
};

