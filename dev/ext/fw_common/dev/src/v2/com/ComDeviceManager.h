/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComDeviceManager.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes

#include <log/Loggable.h>
#include <v2/com/ComDevice.h>
#include <v2/vector/VectorFuncGeneralAuth.h>
class IntComMsg;
class ComLinkManager;
class ComDevice;
class GenComLink;
struct ProtMsg;

// -- Qt includes
#include <QObject>
/**
 * @brief The ComDeviceManager class
 *
 *
 *
 */
class ComDeviceManager : public QObject, public Loggable {
    Q_OBJECT

    //Q_PROPERTY(quint32 state READ getState NOTIFY evStateChanged)

public :

    ComDeviceManager(QObject *parent, IntComMsg * p_pIntComMsg);
    ~ComDeviceManager();

    // -- Configure the system
    ComLinkManager * getLinkManager() const;

    ComDevice* makeDeviceSerial(quint32 p_port, quint32 p_bauds); ///< used in Simu (as Device)
    bool startDeviceTcpServer(quint32 p_port);                   ///< used in Simu (as Device)
    void stopDeviceTcpServer();
    void startDeviceDiscover(); ///< used by MPC
    void stopDeviceDiscover();  ///< used by MPC
    void addDevice(GenComLink* p_link, const VectorFuncGeneralAuth& p_param);   ///< used by MPC (called by LinkManager when a new connection has been authentified)

    //void makeDeviceAndFuntions(const QString &p_deviceName, quint32 p_deviceAddress, const QList<quint32>& p_functions);  ///< configure the map of functions
    //void makeDeviceAndFuntion(const QString &p_deviceName, quint32 p_deviceAddress, quint32 p_function);  ///< configure the map of functions
    void clearDeviceFunctions();
    void removeDevice(ComDevice* p_device);
    QList<quint32> getListFunctions(void) const;            ///< return a list of all functions implemented in the devices
    const QList<ComDevice* > getListDevices(void) const;    ///< return a list of all functions implemented in the devices

    ComDevice * getDevice(quint32 p_function);

    // -- Communicate with devices
    void sendMsg(ProtMsg * p_pMsg, bool p_isEcho = false);
    void emergencyStop();

    // -- Behaviour
    //quint32 getState() const;
    void resetComOnGeneralParamChanged();   ///<

    // -- Topo
//    void makeNetworkTopology(quint32 p_topo);   ///< update links
//    quint32 getNetworkTopoloty() const;

    bool mapFunctionToDevice(quint32 p_funcId, ComDevice *p_pDevice);
    bool unmapFunctionToDevice(quint32 p_funcId);
    void removeDeviceNotMappedToFunction();

    Q_INVOKABLE QStringList getDeviceListInfo() const;

signals :
    // -- State of th Connection
//    void evStateChanged(quint32);   ///< value in COM_FUNC_MANAGER_STATE
    void evDeviceListChanged(qint32 p_value);

private slots :
    void onNewLinkAuthentified(GenComLink *);
    void onLinkDisconnected(GenComLink *);

private :
    // -- Attributs
    QMap<quint32, ComDevice *> m_mapFunctionMapped;   ///< list of functions with the corresponding device
    QList<ComDevice *> m_listDevice;
    QMutex * m_mMapFunction;

    IntComMsg * m_pIntComMsg; ///< reference to my IntComMsg (my owner)
    ComLinkManager * m_pComLinkManager; ///< reference to the ComLinkManager of my IntComMsg

    // -- Behaviour
//    quint32 m_state;    ///< value in COM_FUNC_MANAGER_STATE
//    void updateState();
//    void setState(quint32 p_value);
};
