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

    Q_PROPERTY(quint32 SelectedAScanIndex READ getSelectedAScanIndex WRITE setSelectedAScanIndex NOTIFY evSelectedAScanIndexChanged)
    Q_PROPERTY(VectorFuncAScan* SelectedFuncAScan READ getSelectedFuncAScan NOTIFY evSelectedFuncAScanChanged)            

public :
    ScanManager();
    ~ScanManager();

    void init();
    void initQmlObject(QObject *p_pQmlTopLevel);

    quint32 getSelectedAScanIndex() const;
    void setSelectedAScanIndex(quint32 p_index); ///< value in 0..3

    VectorFuncAScan* getSelectedFuncAScan() const;

    AScan* getCurrentAScan();

    Q_INVOKABLE void ascanZoomIn();
    Q_INVOKABLE void ascanZoomOut();

    Q_INVOKABLE qreal getAScanThicknessMmMultiEcho();
    Q_INVOKABLE qreal getAScanThicknessMm1Echo();

signals :
    void evSelectedAScanIndexChanged(quint32);
    void evSelectedFuncAScanChanged(VectorFuncAScan*);
    void evUpdateAScan();   ///< new AScan to display

private slots :
    void onNewAScanTreated(AScan*);
    void onVectorNbAScanEnabledChanged(qint32);

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

