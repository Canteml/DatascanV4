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
class AScan;
class AScanGraph;
class BScanGraph;
class JkGate;
class ExamManager;


// -- Qt Includes
#include <QObject>
class QQmlApplicationEngine;

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
    Q_PROPERTY(qreal BScanAxeXScaleMm READ getBScanAxeXScaleMm WRITE setBScanAxeXScaleMm NOTIFY evBScanAxeXScaleMmChanged)
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

    qreal getBScanAxeXScaleMm(void) const;
    void setBScanAxeXScaleMm(qreal p_value);

    qreal getBScanAxeYMinMm(void) const;
    void setBScanAxeYMinMm(qreal p_value);

    qreal getBScanAxeYMaxMm(void) const;
    void setBScanAxeYMaxMm(qreal p_value);

    Q_INVOKABLE void bscanAxeXScaleIn();
    Q_INVOKABLE void bscanAxeXScaleOut();

//    Q_INVOKABLE void bscanZoneAdd();
//    Q_INVOKABLE void bscanZoneRemove();

signals :
    void evSelectedAScanIndexChanged(quint32);
    void evSelectedFuncAScanChanged(VectorFuncAScan*);
    void evUpdateAScan();   ///< new AScan to display

    void evBScanViewXModeChanged(qint32);
    void evBScanViewYModeChanged(qint32);
    void evBScanAxeXScaleMmChanged(qreal);
    void evBScanAxeYMinMmChanged(qreal);
    void evBScanAxeYMaxMmChanged(qreal);

private slots :
    void onNewAScanTreated(AScan*);
    void onVectorNbAScanEnabledChanged(qint32);

    void onPrivateBScanViewXModeChanged(qint32);
    void onPrivateBScanViewYModeChanged(qint32);
    void onPrivateBScanAxeXScaleMmChanged(qreal);
    void onPrivateBScanAxeYMinMmChanged(qreal);
    void onPrivateBScanAxeYMaxMmChanged(qreal);

protected :
    // -- Settings
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    void onVectorProductChanged();    
    void setPrivateSelectedAScanIndex(quint32 p_index); ///< value in 0..3

    // -- Behaviour
    quint32 m_selectedAScanIndex;   ///< value in 0..3

    JkGate * m_pGate0;
    JkGate * m_pGate1;

    // -- Manager
    AScan m_pAScan[VectorStd::NB_FUNC_ASCAN];
    AScanGraph* m_pAScanGraphMain;
    BScanGraph* m_pBScanGraphMain;
    VectorStd * m_pVector;
    ExamManager * m_pExamManager;
};

