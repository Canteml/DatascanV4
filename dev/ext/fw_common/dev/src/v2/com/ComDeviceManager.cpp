/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComDeviceManager.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComLinkManager.h>
#include <v2/com/IntComMsg.h>
#include <v2/com/ComDevice.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/GenComLink.h>

// -- Qt includes
#include <QMutex>

ComDeviceManager::ComDeviceManager(QObject *parent, IntComMsg * p_pIntComMsg) : QObject(parent), Loggable("ComDeviceManager")
{
    m_mMapFunction = new QMutex();

    m_pIntComMsg = p_pIntComMsg;
    m_pComLinkManager = p_pIntComMsg->getLinkManager();

    connect(m_pComLinkManager, SIGNAL(evNewLinkAuthentified(GenComLink*)), this, SLOT(onNewLinkAuthentified(GenComLink*)));
    connect(m_pComLinkManager, SIGNAL(evLinkDisconnected(GenComLink*)), this, SLOT(onLinkDisconnected(GenComLink*)));
}

ComDeviceManager::~ComDeviceManager()
{
    clearDeviceFunctions();
}

// -- Configure the system
ComLinkManager * ComDeviceManager::getLinkManager() const
{
    return m_pComLinkManager;
}

ComDevice* ComDeviceManager::makeDeviceSerial(quint32 p_port, quint32 p_bauds)
{
    ComDevice* pDevice = nullptr;
    const VectorFuncGeneralAuth& param = m_pIntComMsg->getFuncGeneralParam();
    if( m_pIntComMsg->getDeviceAd() == ProtMsg::DEVICE_AD_MPC ) {
        fLogError("MPC can't make a device by this way");
    } else {
        if( !m_listDevice.isEmpty() ) {
            fLogWarning(QString("This vector has already Devices (%1)").arg(m_listDevice.size()));
        }
        clearDeviceFunctions();
        // instanciate a serial port com
        GenComLink* pLink = m_pComLinkManager->addSerial(p_port, p_bauds, QString("Serial%1").arg(p_port));
        // make a device
        m_mMapFunction->lock();
        pDevice = new ComDevice(m_pIntComMsg, pLink, param);
        // declare this device for the associated functions
        for(auto i : qAsConst(param.m_lFunctions) ) {
            if( !m_mapFunctionMapped.contains(i) ) {
                m_mapFunctionMapped.insert(i ,pDevice);
            } else {
                fLogWarning(QString("Function %1 already mapped to a device").arg(i));
            }
        }
        m_listDevice.append(pDevice);
        m_mMapFunction->unlock();
        emit evDeviceListChanged(m_listDevice.size());
    }
    return pDevice;
}

bool ComDeviceManager::startDeviceTcpServer(quint32 p_port)
{
    // the TCP Server is used
    if( m_pIntComMsg->getDeviceAd() == ProtMsg::DEVICE_AD_MPC ) {
        fLogError("MPC can't make a device by this way");
    } else {
        return m_pComLinkManager->startTcpServer(p_port);
    }
    return false;
}

void ComDeviceManager::stopDeviceTcpServer()
{
    m_pComLinkManager->stopTcpServer();
}

void ComDeviceManager::startDeviceDiscover()
{
    if( m_pIntComMsg->getDeviceAd() != ProtMsg::DEVICE_AD_MPC ) {
        fLogError("Only MPC can discover devices");
    } else {
        m_pComLinkManager->startDiscover();
    }
}

void ComDeviceManager::stopDeviceDiscover()
{
    m_pComLinkManager->stopDiscover();
}

//void ComDeviceManager::makeDeviceAndFuntions(const QString & p_deviceName, quint32 p_deviceAddress, const QList<quint32> &p_functions)
//    ComDevice* pDevice = new ComDevice(m_pIntComMsg, m_pIntComMsg->getProdId(), p_deviceAddress);   // TODO : should not be done here, but in device discovery process, after the link response to "WHO_ARE_YOU"

//    // Find if there is a Device with the same name that the links :

//void ComDeviceManager::makeDeviceAndFuntion(const QString & p_deviceName, quint32 p_deviceAddress, quint32 p_function)

void ComDeviceManager::clearDeviceFunctions()
{
    // Delete config   
    while (m_listDevice.size()) {
        delete m_listDevice.takeFirst();
    }
    m_mMapFunction->lock();
    m_mapFunctionMapped.clear(); // forget map of functions
    m_mMapFunction->unlock();
    m_pComLinkManager->deleteLinks();
    emit evDeviceListChanged(m_listDevice.size());
}

void ComDeviceManager::removeDevice(ComDevice *p_device)
{
    m_mMapFunction->lock();
    m_listDevice.removeAll(p_device);
    // unmap functions associated to this device
    bool bRemove = true;
    while(bRemove) {
        qint32 k = m_mapFunctionMapped.key(p_device, -1);
        if( -1 != k ) {
            m_mapFunctionMapped.remove(k);
        } else {
            bRemove = false;
        }
    }
    m_mMapFunction->unlock();
    delete p_device;
    emit evDeviceListChanged(m_listDevice.size());
}

QList<quint32> ComDeviceManager::getListFunctions() const
{
    QList<quint32> l;
    m_mMapFunction->lock();
    l = m_mapFunctionMapped.keys();
    m_mMapFunction->unlock();
    return l;
}

const QList<ComDevice *> ComDeviceManager::getListDevices() const
{
    return m_listDevice;
}

ComDevice * ComDeviceManager::getDevice(quint32 p_function)
{
    ComDevice * retComp = nullptr;
    m_mMapFunction->lock();
    if( m_mapFunctionMapped.contains(p_function) ) {
        retComp = m_mapFunctionMapped.value(p_function);
    }
    m_mMapFunction->unlock();
    return retComp;
}

// -- Communicate with Device
void ComDeviceManager::sendMsg(ProtMsg * p_pMsg, bool p_isEcho)
{
    m_mMapFunction->lock();
    if( !p_isEcho && p_pMsg->isParamDevice()) {
        // Send the message to the device of this function only if it's not an answer
        // to a SET msg send by this device.
        if( m_mapFunctionMapped.contains(p_pMsg->m_deviceFunc) ) {
            m_mapFunctionMapped.value(p_pMsg->m_deviceFunc)->sendMsg(p_pMsg);
        } else {
            fLogWarning(QString("No device found for function %1").arg(p_pMsg->m_deviceFunc));
            delete p_pMsg;
        }
        // TODO : send the message to the item of ControlPanel that havec subscribed to this function.
    } else {
        delete p_pMsg;
    }
    m_mMapFunction->unlock();
}

void ComDeviceManager::emergencyStop()
{

    m_mMapFunction->lock();
    // send emergency stop only to deveices mapped to functions
    for(auto d : qAsConst(m_listDevice)) {
        bool bDeviceUsed = false;
        for(auto f : qAsConst(m_mapFunctionMapped) ) {
            if( f == d ) {
                bDeviceUsed = true;
                break;
            }
        }
        if( bDeviceUsed ) {
            d->emergencyStop();
        }
    }

    m_mMapFunction->unlock();
}

void ComDeviceManager::resetComOnGeneralParamChanged()
{
    // TODO : DEVICES / SIMU : restart com.
}

// -- Behaviour
bool ComDeviceManager::mapFunctionToDevice(quint32 p_funcId, ComDevice *p_pDevice)
{
    bool bRet = true;
    if( !m_mapFunctionMapped.contains(p_funcId) ) {
        m_mapFunctionMapped.insert(p_funcId, p_pDevice);
    } else {
        bRet = false;
        fLogError(QString("Trying to add a function that is already mapped (%1)").arg(p_funcId));
    }
    return bRet;
}

bool ComDeviceManager::unmapFunctionToDevice(quint32 p_funcId)
{
    if( m_mapFunctionMapped.contains(p_funcId) ) {
        m_mapFunctionMapped.remove(p_funcId);
        return true;
    }
    return false;
}

void ComDeviceManager::removeDeviceNotMappedToFunction()
{    
    m_mMapFunction->lock();
    for(auto d : qAsConst(m_listDevice)) {
        bool bDeviceUsed = false;
        for(auto f : qAsConst(m_mapFunctionMapped) ) {
            if( f == d ) {
                bDeviceUsed = true;
                break;
            }
        }
        if( !bDeviceUsed ) {
            m_listDevice.removeAll(d);
            delete d;
        }
    }    
    m_mMapFunction->unlock();
    stopDeviceDiscover();
    emit evDeviceListChanged(m_listDevice.size());
}

QStringList ComDeviceManager::getDeviceListInfo() const
{
    QStringList sl;    
    m_mMapFunction->lock();
    for(auto d : qAsConst(m_listDevice)) {
        bool bDeviceUsed = false;
        for(auto f : qAsConst(m_mapFunctionMapped) ) {
            if( f == d ) {
                bDeviceUsed = true;
                break;
            }
        }
        if( bDeviceUsed ) {
            sl.append(QString(tr("■ %1%2\tVersion matériel : %3\t\t\tVersion logiciel : %4\n\t\tDate création : %5 \tDate révision : %6"))
                    .arg(d->getParamDProdUid(),QString(20-d->getParamDProdUid().length(), QChar(' ')),d->getParamDVerHw(),d->getParamDVerSw(),
                    QDateTime().fromSecsSinceEpoch(d->getParamDDateBuild()).toString(),
                    QDateTime().fromSecsSinceEpoch(d->getParamDDateRev()).toString()));
        }
    }    
    m_mMapFunction->unlock();
    return sl;
}

// -- Connection state of the system

void ComDeviceManager::onNewLinkAuthentified(GenComLink * p_pLink)
{
    const VectorFuncGeneralAuth funcGen = p_pLink->getFuncGeneral();

    // make a device
    m_mMapFunction->lock();
    ComDevice * pDevice = new ComDevice(m_pIntComMsg, p_pLink, funcGen);

    // declare this device for the associated functions
    for(auto i : qAsConst(funcGen.m_lFunctions) ) {
        if( !m_mapFunctionMapped.contains(i) ) {
            m_mapFunctionMapped.insert(i ,pDevice);
        } else {
            fLogWarning(QString("Function %1 already mapped to a device").arg(i));
        }
    }
    m_listDevice.append(pDevice);
    m_mMapFunction->unlock();

    emit evDeviceListChanged(m_listDevice.size());

//    // TODO :
//    // - put the link in a list of links to bind with new Device :
//    //      => send a msg "WHO_ARE_YOU"
//    //      => make devices, bind them with a link
//    // - after a bind, add Comp. To a list of CompDiscovered.
//    // - afer a link disconnection or a loose of heart beat, delete a CompDiscovered (and remove it from the map)

//    // For now : map directly the new link to the first map item.
//        // Find if there is a Device with the same name that the link :
}

void ComDeviceManager::onLinkDisconnected(GenComLink * m_pLink)
{
    // remove device associated to the link :
    ComDevice * pAssociatedDevice = nullptr;
    for(auto i : qAsConst(m_listDevice)) {
        if( i->getLink() == m_pLink ) {
            pAssociatedDevice = i;
            break;
        }
    }

    if( nullptr != pAssociatedDevice ) {
        removeDevice(pAssociatedDevice);
    }

    emit evDeviceListChanged(m_listDevice.size());
}

//void ComDeviceManager::updateState()

// -- Beheviour
//void ComDeviceManager::setState(quint32 p_value)
