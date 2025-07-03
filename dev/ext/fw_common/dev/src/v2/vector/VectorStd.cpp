/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorStd.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkSerial.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComLinkManager.h>

#include <v2/vector/VectorFuncPosition.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/vector/VectorFuncMove.h>
#include <v2/vector/VectorFuncPump.h>
#include <v2/vector/VectorFuncLight.h>
#include <v2/vector/VectorFuncInertialSensor.h>
#include <v2/vector/VectorFuncPressureSensor.h>
#include <v2/vector/VectorFuncTemperatureSensor.h>
#include <v2/vector/VectorFuncHumiditySensor.h>
#include <v2/vector/VectorFuncBattery.h>
#include <v2/vector/VectorFuncCleaning.h>
#include <v2/vector/VectorFuncEndStop.h>
#include <v2/vector/VectorFuncLicence.h>
#include <v2/vector/VectorFuncCompass.h>

VectorStd::VectorStd(QObject *parent) : IntComMsg(parent)
{
    setLogName("VectorStd");
    setXmlSectionName("VectorStd");
    setFunctionStateConnection(FUNC_CONNECTION_DISCONNECTED);
    connect(m_pComDeviceManager, &ComDeviceManager::evDeviceListChanged, this, &VectorStd::onDeviceManagerListChanged);

    // Instantiate functions :

    m_pFuncPosition = new VectorFuncPosition(this);
    m_mapFunctionsById.insert(m_pFuncPosition->getFuncId(), m_pFuncPosition);

    for(int i=0; i< NB_FUNC_ASCAN; i++ ) {
        m_pFuncAScan[i] = new VectorFuncAScan(this, i);
        m_mapFunctionsById.insert(m_pFuncAScan[i]->getFuncId(), m_pFuncAScan[i]);
    }

    m_pFuncMove = new VectorFuncMove(this);
    m_mapFunctionsById.insert(m_pFuncMove->getFuncId(), m_pFuncMove);

    m_pFuncPump = new VectorFuncPump(this);
    m_mapFunctionsById.insert(m_pFuncPump->getFuncId(), m_pFuncPump);

    m_pFuncLight = new VectorFuncLight(this);
    m_mapFunctionsById.insert(m_pFuncLight->getFuncId(), m_pFuncLight);

    m_pFuncInertialSensor = new VectorFuncInertialSensor(this);
    m_mapFunctionsById.insert(m_pFuncInertialSensor->getFuncId(), m_pFuncInertialSensor);

    for(int i=0; i< NB_FUNC_PRESSURE; i++ ) {
        m_pFuncPressureSensor[i] = new VectorFuncPressureSensor(this, i);
        m_mapFunctionsById.insert(m_pFuncPressureSensor[i]->getFuncId(), m_pFuncPressureSensor[i]);
    }

    for(int i=0; i< NB_FUNC_TEMPERATURE; i++ ) {
        m_pFuncTemperatureSensor[i] = new VectorFuncTemperatureSensor(this, i);
        m_mapFunctionsById.insert(m_pFuncTemperatureSensor[i]->getFuncId(), m_pFuncTemperatureSensor[i]);
    }

    for(int i=0; i< NB_FUNC_HUMIDITY; i++ ) {
        m_pFuncHumiditySensor[i] = new VectorFuncHumiditySensor(this, i);
        m_mapFunctionsById.insert(m_pFuncHumiditySensor[i]->getFuncId(), m_pFuncHumiditySensor[i]);
    }

    m_pFuncBattery = new VectorFuncBattery(this);
    m_mapFunctionsById.insert(m_pFuncBattery->getFuncId(), m_pFuncBattery);

    m_pFuncCleaning = new VectorFuncCleaning(this);
    m_mapFunctionsById.insert(m_pFuncCleaning->getFuncId(), m_pFuncCleaning);

    m_pFuncEndStop = new VectorFuncEndStop(this);
    m_mapFunctionsById.insert(m_pFuncEndStop->getFuncId(), m_pFuncEndStop);

    m_pFuncLicence = new VectorFuncLicence(this);
    m_mapFunctionsById.insert(m_pFuncLicence->getFuncId(), m_pFuncLicence);

    m_pFuncCompass = new VectorFuncCompass(this);
    m_mapFunctionsById.insert(m_pFuncCompass->getFuncId(), m_pFuncCompass);
}

VectorStd::~VectorStd()
{    
    disconnect(m_pComDeviceManager, &ComDeviceManager::evDeviceListChanged, this, &VectorStd::onDeviceManagerListChanged);
    destroyVector();
}

// -- Methods inherited from IntComMsg :
void VectorStd::processReceivedMsg(ProtMsg* p_pMsg)
{
//    fLogInfo(QString("Received MSG : DEVICE_AD(%1) MSG_TYPE(%2) DEVICE_FUNC(%3) MSG_OBJ(%4)")
//             .arg(p_pMsg->m_deviceAd)
//             .arg(p_pMsg->m_msgType)
//             .arg(p_pMsg->m_deviceFunc)
//             .arg(p_pMsg->m_msgObj));
    if( m_mapFunctionsById.contains(p_pMsg->m_deviceFunc) ) {
        GenVectorFunc * pFunc = m_mapFunctionsById.value(p_pMsg->m_deviceFunc);
        // pFunc->processReceivedMsg(p_pMsg);
        pFunc->pushMsg(p_pMsg);
    } else {
        fLogWarning(QString("Message not mapped : COMP(%1) MSG_TYPE(%2) MSG_GRP(%3) MSG_OBJ(%4)")
                 .arg(p_pMsg->m_deviceAd)
                 .arg(p_pMsg->m_msgType)
                 .arg(p_pMsg->m_deviceFunc)
                 .arg(p_pMsg->m_msgObj));
    }
}

void VectorStd::destroyVector()
{
    disableAllFunctions();

    m_mapFunctionsById.clear();

    for(int i=0; i<NB_FUNC_ASCAN; i++ ) {
        if( nullptr != m_pFuncAScan[i] ) {
            delete m_pFuncAScan[i];
            m_pFuncAScan[i] = nullptr;
        }
    }

    if( nullptr != m_pFuncPosition ) {
        delete m_pFuncPosition;
        m_pFuncPosition = nullptr;
    }
    if( nullptr != m_pFuncMove ) {
        delete m_pFuncMove;
        m_pFuncMove = nullptr;
    }
    if( nullptr != m_pFuncPump ) {
        delete m_pFuncPump;
        m_pFuncPump = nullptr;
    }
    if( nullptr != m_pFuncLight ) {
        delete m_pFuncLight;
        m_pFuncLight = nullptr;
    }
    if( nullptr != m_pFuncInertialSensor ) {
        delete m_pFuncInertialSensor;
        m_pFuncInertialSensor = nullptr;
    }
    for(int i=0; i<NB_FUNC_PRESSURE; i++ ) {
        if( nullptr != m_pFuncPressureSensor[i] ) {
            delete m_pFuncPressureSensor[i];
            m_pFuncPressureSensor[i] = nullptr;
        }
    }
    for(int i=0; i<NB_FUNC_TEMPERATURE; i++ ) {
        if( nullptr != m_pFuncTemperatureSensor[i] ) {
            delete m_pFuncTemperatureSensor[i];
            m_pFuncTemperatureSensor[i] = nullptr;
        }
    }
    for(int i=0; i<NB_FUNC_HUMIDITY; i++ ) {
        if( nullptr != m_pFuncHumiditySensor[i] ) {
            delete m_pFuncHumiditySensor[i];
            m_pFuncHumiditySensor[i] = nullptr;
        }
    }
    if( nullptr != m_pFuncBattery ) {
        delete m_pFuncBattery;
        m_pFuncBattery = nullptr;
    }
    if( nullptr != m_pFuncCleaning ) {
        delete m_pFuncCleaning;
        m_pFuncCleaning = nullptr;
    }
    if( nullptr != m_pFuncEndStop ) {
        delete m_pFuncEndStop;
        m_pFuncEndStop = nullptr;
    }
    if( nullptr != m_pFuncLicence ) {
        delete m_pFuncLicence;
        m_pFuncLicence = nullptr;
    }
    if( nullptr != m_pFuncCompass ) {
        delete m_pFuncCompass;
        m_pFuncCompass = nullptr;
    }
}

void VectorStd::disableAllFunctions()
{
    for(auto i : qAsConst(m_mapFunctionsById)) {
        i->setIsEnabled(false);
    }
    emit evListFunctionEnabledChanged();
}

void VectorStd::emergencyStop() const
{
    m_pComDeviceManager->emergencyStop();
}

void VectorStd::clearAScan()
{
    for(int i=0; i<NB_FUNC_ASCAN; i++) {
        m_pFuncAScan[i]->clearAScan();
    }
}

// -- MODEL COMP

VectorFuncAScan * VectorStd::funcAScan0() const
{
    return m_pFuncAScan[0];
}

VectorFuncAScan * VectorStd::funcAScan1() const
{
    return m_pFuncAScan[1];
}

VectorFuncAScan * VectorStd::funcAScan2() const
{
    return m_pFuncAScan[2];
}

VectorFuncAScan * VectorStd::funcAScan3() const
{
    return m_pFuncAScan[3];
}

VectorFuncPosition * VectorStd::funcPosition() const
{
    return m_pFuncPosition;
}

VectorFuncMove * VectorStd::funcMove() const
{
    return m_pFuncMove;
}

VectorFuncPump * VectorStd::funcPump() const
{
    return m_pFuncPump;
}

VectorFuncLight * VectorStd::funcLight() const
{
    return m_pFuncLight;
}

VectorFuncInertialSensor * VectorStd::funcInertialSensor() const
{
    return m_pFuncInertialSensor;
}

VectorFuncPressureSensor * VectorStd::funcPressureSensor0() const
{
    return m_pFuncPressureSensor[0];
}

VectorFuncPressureSensor * VectorStd::funcPressureSensor1() const
{
    return m_pFuncPressureSensor[1];
}

VectorFuncPressureSensor * VectorStd::funcPressureSensor2() const
{
    return m_pFuncPressureSensor[2];
}

VectorFuncTemperatureSensor * VectorStd::funcTemperatureSensor0() const
{
    return m_pFuncTemperatureSensor[0];
}

VectorFuncTemperatureSensor * VectorStd::funcTemperatureSensor1() const
{
    return m_pFuncTemperatureSensor[1];
}

VectorFuncTemperatureSensor * VectorStd::funcTemperatureSensor2() const
{
    return m_pFuncTemperatureSensor[2];
}

VectorFuncHumiditySensor * VectorStd::funcHumiditySensor0() const
{
    return m_pFuncHumiditySensor[0];
}

VectorFuncHumiditySensor * VectorStd::funcHumiditySensor1() const
{
    return m_pFuncHumiditySensor[1];
}

VectorFuncHumiditySensor * VectorStd::funcHumiditySensor2() const
{
    return m_pFuncHumiditySensor[2];
}

VectorFuncBattery *VectorStd::funcBattery() const
{
    return m_pFuncBattery;
}

VectorFuncCleaning *VectorStd::funcCleaning() const
{
    return m_pFuncCleaning;
}

VectorFuncEndStop *VectorStd::funcEndStop() const
{
    return m_pFuncEndStop;
}

VectorFuncLicence *VectorStd::funcLicence() const
{
    return m_pFuncLicence;
}

VectorFuncCompass *VectorStd::funcCompass() const
{
    return m_pFuncCompass;
}

VectorFuncAScan *VectorStd::getFuncAScan(qint32 p_index) const
{
    if( 0 <=p_index && p_index<NB_FUNC_ASCAN ) {
        return m_pFuncAScan[p_index];
    }
    fLogError(QString("Bad index (0..3) : %1").arg(p_index));
    return nullptr;
}

qint32 VectorStd::getNbFuncAScanEnabled() const
{
    int iRet = 0;
    for(int i=0; i<NB_FUNC_ASCAN; i++) {
        if( m_pFuncAScan[i]->getIsEnabled() ) {
            iRet++;
        }
    }
    return iRet;
}

qint32 VectorStd::getFuncStateConnection() const
{
    return m_funcStateConnection;
}


void VectorStd::setListFunctionEnabled(const QList<quint32>& p_lFunc)
{
    // Delete Devices and links
    disableAllFunctions();
    m_pComDeviceManager->clearDeviceFunctions();

    // Enable specified functions
    for(auto i : p_lFunc ) {
        m_mapFunctionsById.value(i)->configure(true, m_funcGeneral.m_paramDProdId);
    }

    // update function connected :
    onDeviceManagerListChanged();
    emit evNbFuncAScanEnabledChanged(getNbFuncAScanEnabled());

    // remove useless connections
    m_pComDeviceManager->removeDeviceNotMappedToFunction();

    emit evListFunctionEnabledChanged();
}

void VectorStd::setProdIdDeviceAdFuntions(quint32 p_prodId, quint32 p_deviceAd, const QList<quint32> &p_lFun)
{
        // configure Vector :
        if( ProtMsg::DEVICE_AD_MPC == p_deviceAd ) {
            setLogName("VectorStd(MPC)");
        } else {
            setLogName(QString("VectorStd(%1)").arg(p_deviceAd));
        }

        setListFunctionEnabled(p_lFun);

        emit prodIdChanged(p_prodId);
}

QList<quint32> VectorStd::getListFunctionEnabled() const
{
    QList<quint32> lRet;
    for(auto f : m_mapFunctionsById ) {
        if( f->getIsEnabled() ) {
            lRet.append(f->getFuncId());
        }
    }
    return lRet;
}

QStringList VectorStd::getListFunctionString() const
{
    QStringList sl;
    for(auto i : qAsConst(m_mapFunctionsById) ) {
        sl.append(i->getFunctionName());
    }
    return sl;
}

void VectorStd::onDeviceManagerListChanged()
{
    // request to each function to check their connection state
    int nbConnected = 0;
    int nbNotConnected = 0;
    for(auto i : qAsConst(m_mapFunctionsById) ) {
        bool bConnected = i->updateConnectedState();
        if( i->getIsEnabled() ) {
            if( bConnected ) {
                nbConnected ++;
            } else {
                nbNotConnected++;
            }
        }
    }

    if( nbConnected == 0 ) {
        setFunctionStateConnection(FUNC_CONNECTION_DISCONNECTED);
    } else {
        if( 0 == nbNotConnected ) {
            setFunctionStateConnection(FUNC_CONNECTION_CONNECTED);
        } else {
            setFunctionStateConnection(FUNC_CONNECTION_PARTIALY_CONNECTED);
        }
    }
}

// -- Inherited from XmlProject
bool VectorStd::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= IntComMsg::customLoadParam(p_pParent);

    QList<GenVectorFunc*> lFunc = m_mapFunctionsById.values();
    for(auto i : qAsConst(lFunc)) {
        i->loadParam(p_pParent);
    }

    return bRet;
}

bool VectorStd::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= IntComMsg::customLoadDefaultParam();
    return bRet;
}

bool VectorStd::customApplyParam()
{
    bool bRet = true;   
    bRet &= IntComMsg::customApplyParam();

    configureProduct(m_funcGeneral.m_paramDProdId, m_funcGeneral.m_paramDDeviceAd);
    // auto start Discover for MPC
    if( isMpc() ) {
        m_pComDeviceManager->startDeviceDiscover();
    }

    return bRet;
}

bool VectorStd::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;

    bRet &= IntComMsg::customSaveParam(p_pDoc, p_pParent);

    QList<GenVectorFunc*> lFunc = m_mapFunctionsById.values();
    for(auto i : qAsConst(lFunc)) {
        i->saveParam(p_pDoc, p_pParent);
    }

    return bRet;
}


// -- Behaviour

void VectorStd::configureProduct(quint32 p_prodId, quint32 p_deviceAd)
{
    // enable "functions" if nedeed by a product
    QList<quint32> lFunc;

    // Enable ASCAN0 for some products
    if( (ProtMsg::PROD_UT_POLE == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V14 == p_prodId) ||
        (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_ROVER_IP68 == p_prodId) ||
        (ProtMsg::PROD_PANDA_VOL3 == p_prodId)||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_ASCAN_0);
    }

    // Enable ASCAN1 & ASCAN2 for some products
    if( (ProtMsg::PROD_PANDA_VOL3 == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_ASCAN_1);
        lFunc.append(ProtMsg::DEVICE_FUNC_ASCAN_2);
    }

    // Enable ASCAN3 for some products
    if( (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_ASCAN_3);
    }

    // Enable POSITION for some products
    if( (ProtMsg::PROD_UT_POLE == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V14 == p_prodId) ||
        (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_ROVER_IP68 == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_POSITION);
    }

    // Enable MOVE for some products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V14 == p_prodId) ||
        (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_ROVER_IP68 == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_MOVE);
    }

    // Enable PUMP for some products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_PUMP);
    }

    // Enable LIGHT for all products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_LIGHT);
    }

    // Enable INERTIAL for some products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ROVER_IP68 == p_prodId) ||
        (ProtMsg::PROD_MULTISCAN == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_INERTIAL_SENSOR);
    }

    // Enable PRESSURE for some products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ROVER_IP68 == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_PRESSURE_SENSOR_0);
    }

    // All products have a battery
    lFunc.append(ProtMsg::DEVICE_FUNC_BATTERY);

    // Enable CLEANING for some products
    if( (ProtMsg::PROD_ROVER == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_CLEANING);
    }

    // Enable temperature and humidity sensors for some products
    if( (ProtMsg::PROD_ROVER == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ||
        (ProtMsg::PROD_ALTISCAN_V14 == p_prodId) ||
        (ProtMsg::PROD_UT_POLE == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_TEMPERATURE_SENSOR_0);
        lFunc.append(ProtMsg::DEVICE_FUNC_HUMIDITY_SENSOR_0);
    }

    // Enable END-STOP for some products
    if( (ProtMsg::PROD_ALTISCAN_V2 == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_ENDSTOP);
    }

    // All products have a licence
    lFunc.append(ProtMsg::DEVICE_FUNC_LICENCE);

    // Enable COMPASS for some products
    if( (ProtMsg::PROD_ROVER == p_prodId) ) {
        lFunc.append(ProtMsg::DEVICE_FUNC_COMPASS);
    }

    setProdIdDeviceAdFuntions(p_prodId, p_deviceAd, lFunc);

}

void VectorStd::setFunctionStateConnection(qint32 p_value)
{
    if( m_funcStateConnection != p_value ) {
        m_funcStateConnection = p_value;
        emit evFuncStateConnectionChanged(m_funcStateConnection);
    }
}
