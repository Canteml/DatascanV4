/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorStd.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/com/IntComMsg.h>
#include <v2/vector/VectorFunc.h>
#include <xml/XmlProject.h>
#include "VectorFuncAScan.h"
#include "VectorFuncPosition.h"
#include "VectorFuncMove.h"
#include "VectorFuncPump.h"
#include "VectorFuncLight.h"
#include "VectorFuncInertialSensor.h"
#include "VectorFuncPressureSensor.h"
#include "VectorFuncTemperatureSensor.h"
#include "VectorFuncHumiditySensor.h"
#include "VectorFuncBattery.h"
#include "VectorFuncCleaning.h"
#include "VectorFuncEndStop.h"
#include "VectorFuncLicence.h"
#include "VectorFuncCompass.h"
#include <QMetaType>

class GenVectorFunc;

// -- Qt include
#include <QMap>

/**
 * @brief The VectorStd class
 *
 * \todo Camera management
 * - Create a function to manage all camera
 * - Camera roles are :
 *      - ASCAN : true / false => if true, add one camera for eache ASCAN available
 *      - REAR : view behind the vector
 *      - SECURITY : view the explosimeter and lights
 *
 */
class VectorStd : public IntComMsg {
    Q_OBJECT

    // -- INTERNAL PROPERTIES
    Q_PROPERTY(qint32 NbFuncAScanEnabled READ getNbFuncAScanEnabled NOTIFY evNbFuncAScanEnabledChanged)
    Q_PROPERTY(qint32 FuncStateConnection READ getFuncStateConnection NOTIFY evFuncStateConnectionChanged)

    QML_ELEMENT

public :
    VectorStd(QObject* parent);
    ~VectorStd();

    // -- Methods inherited from IntComMsg :
    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;   ///< this method will be called be all devices with theire received messages. (TODO : private method)

    // -- INTERNAL PROPERTIES

    // -- Behaviour
    void destroyVector();
    void disableAllFunctions();
    Q_INVOKABLE void emergencyStop() const; ///< request an emergency stop for all devices

    void clearAScan();///< remove all remaining data in AScan functions

    // -- List of functions
    Q_INVOKABLE VectorFuncAScan * funcAScan0() const;
    Q_INVOKABLE VectorFuncAScan * funcAScan1() const;
    Q_INVOKABLE VectorFuncAScan * funcAScan2() const;
    Q_INVOKABLE VectorFuncAScan * funcAScan3() const;
    Q_INVOKABLE VectorFuncPosition * funcPosition() const;
    Q_INVOKABLE VectorFuncMove * funcMove() const;
    Q_INVOKABLE VectorFuncPump * funcPump() const;
    Q_INVOKABLE VectorFuncLight * funcLight() const;
    Q_INVOKABLE VectorFuncInertialSensor * funcInertialSensor() const;
    Q_INVOKABLE VectorFuncPressureSensor * funcPressureSensor0() const;
    Q_INVOKABLE VectorFuncPressureSensor * funcPressureSensor1() const;
    Q_INVOKABLE VectorFuncPressureSensor * funcPressureSensor2() const;
    Q_INVOKABLE VectorFuncTemperatureSensor * funcTemperatureSensor0() const;
    Q_INVOKABLE VectorFuncTemperatureSensor * funcTemperatureSensor1() const;
    Q_INVOKABLE VectorFuncTemperatureSensor * funcTemperatureSensor2() const;
    Q_INVOKABLE VectorFuncHumiditySensor * funcHumiditySensor0() const;
    Q_INVOKABLE VectorFuncHumiditySensor * funcHumiditySensor1() const;
    Q_INVOKABLE VectorFuncHumiditySensor * funcHumiditySensor2() const;
    Q_INVOKABLE VectorFuncBattery * funcBattery() const;
    Q_INVOKABLE VectorFuncCleaning * funcCleaning() const;
    Q_INVOKABLE VectorFuncEndStop * funcEndStop() const;
    Q_INVOKABLE VectorFuncLicence * funcLicence() const;
    Q_INVOKABLE VectorFuncCompass * funcCompass() const;


    // -- Help
    static const qint32 NB_FUNC_ASCAN = 4;
    static const qint32 NB_FUNC_PRESSURE = 3;
    static const qint32 NB_FUNC_TEMPERATURE = 3;
    static const qint32 NB_FUNC_HUMIDITY = 3;
    VectorFuncAScan *getFuncAScan(qint32 p_index) const;
    Q_INVOKABLE qint32 getNbFuncAScanEnabled(void) const;

    // -- Behaviour

    enum FUNC_CONNECTION_STATE {
        FUNC_CONNECTION_DISCONNECTED,
        FUNC_CONNECTION_PARTIALY_CONNECTED,
        FUNC_CONNECTION_CONNECTED
    };

    qint32 getFuncStateConnection() const;
    void setListFunctionEnabled(const QList<quint32> &p_lFunc);
    void setProdIdDeviceAdFuntions(quint32 p_prodId, quint32 p_deviceAd,const QList<quint32>& p_lFun);
    QList<quint32> getListFunctionEnabled() const;
    QStringList getListFunctionString() const;

signals :
    // -- INTERNAL PROPERTIES
    void evNbFuncAScanEnabledChanged(qint32);
    void evListFunctionEnabledChanged();

    // -- Behaviour
    void evFuncStateConnectionChanged(quint32);

private slots :
    void onDeviceManagerListChanged();    ///< called when a device is added / removed.


protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void configureProduct(quint32 p_prodId, quint32 p_deviceAd) override;   ///< this method will be called be all devices with theire received messages. (TODO : private method)

    void setFunctionStateConnection(qint32 p_value);
    qint32 m_funcStateConnection;  ///< value in FUNC_CONNECTION_STATE

    // -- MODEL COMP
    VectorFuncAScan * m_pFuncAScan[NB_FUNC_ASCAN];
    VectorFuncPosition * m_pFuncPosition;
    VectorFuncMove * m_pFuncMove;
    VectorFuncPump * m_pFuncPump;
    VectorFuncLight * m_pFuncLight;
    VectorFuncInertialSensor * m_pFuncInertialSensor;
    VectorFuncPressureSensor * m_pFuncPressureSensor[NB_FUNC_PRESSURE];
    VectorFuncTemperatureSensor * m_pFuncTemperatureSensor[NB_FUNC_TEMPERATURE];
    VectorFuncHumiditySensor * m_pFuncHumiditySensor[NB_FUNC_HUMIDITY];
    VectorFuncBattery * m_pFuncBattery;
    VectorFuncCleaning * m_pFuncCleaning;
    VectorFuncEndStop * m_pFuncEndStop;
    VectorFuncLicence * m_pFuncLicence;
    VectorFuncCompass * m_pFuncCompass;

    QMap<quint32, GenVectorFunc*> m_mapFunctionsById;   ///< Map with all existing functions. Only required functions will be enabled.

    // -- INTERNAL PROPERTIES
};
