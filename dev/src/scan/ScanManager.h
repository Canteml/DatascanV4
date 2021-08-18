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
class JkGate;


// -- Qt Includes
#include <QObject>
class QQmlApplicationEngine;

/**
 * @brief The ScanManager class
 *
 *  TODO : select source of ASCAN :
 *      - BSCAN if user click on it
 *      - VectorFuncAScanX
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

    Q_INVOKABLE void ascanZoomIn();
    Q_INVOKABLE void ascanZoomOut();

signals :
    void evSelectedAScanIndexChanged(quint32);
    void evSelectedFuncAScanChanged(VectorFuncAScan*);

private slots :
    void onNewAScan0Treated(AScan*);
    void onNewAScan1Treated(AScan*);
    void onNewAScan2Treated(AScan*);
    void onNewAScan3Treated(AScan*);
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
    AScan* m_pAScan[VectorStd::NB_FUNC_ASCAN];
    AScanGraph* m_pAScanGraphMain;
    VectorStd * m_pVector;
};

