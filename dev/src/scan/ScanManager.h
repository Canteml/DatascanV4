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
class AScan;
class AScanGraph;
class BScanGraph;
class JkGate;
class ExamManager;


// -- Qt Includes
#include <QObject>
class QQmlApplicationEngine;
class QTimer;

/**
 * @brief The ScanManager class
 *
 *  TODO : select source of ASCAN :
 *      - A ASCAN stored in a BSCAN if user click on BSCAN
 *      - the last ASCAN of the selected sensor
 */

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
    ScanManager();
    ~ScanManager();    

    void init();
    void initQmlObject(QObject *p_pQmlTopLevel);
    void stop();

    quint32 getSelectedAScanIndex() const;
    void setSelectedAScanIndex(quint32 p_index); ///< value in 0..3

    VectorFuncAScan* getSelectedFuncAScan() const;

    AScan* getCurrentAScan();

    // -- ASCAN

    Q_INVOKABLE void ascanZoomIn();
    Q_INVOKABLE void ascanZoomOut();

    Q_INVOKABLE qreal getAScanThicknessMmMultiEcho();
    Q_INVOKABLE qreal getAScanThicknessMm1Echo();

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


    static const qint32 ASCAN_MEASURE_REFRESH_DELAY = 100;


signals :
    void evSelectedAScanIndexChanged(quint32);
    void evSelectedFuncAScanChanged(VectorFuncAScan*);
    void evUpdateAScan();   ///< new AScan to display

    void evBScanViewXModeChanged(qint32);
    void evBScanViewYModeChanged(qint32);
    void evBScanAxeXScaleMChanged(qreal);
    void evBScanAxeYMinMmChanged(qreal);
    void evBScanAxeYMaxMmChanged(qreal);

private slots :
    void onNewAScanTreated(AScan*);
    void onVectorNbAScanEnabledChanged(qint32);

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
    void onVectorProductChanged();    
    void setPrivateSelectedAScanIndex(quint32 p_index); ///< value in 0..3
    void emitUpdateAScanTimed(bool p_force = false);

    // -- Behaviour
    quint32 m_selectedAScanIndex;   ///< value in 0..3
    QTimer * m_timUpdateAScan;
    qint64 m_lastUpdateAScan;

    JkGate * m_pGate0;
    JkGate * m_pGate1;

    // -- Manager
    AScan m_pAScan[VectorStd::NB_FUNC_ASCAN];
    AScanGraph* m_pAScanGraphMain;
    BScanGraph* m_pBScanGraphMain;
    VectorStd * m_pVector;
    ExamManager * m_pExamManager;
};

