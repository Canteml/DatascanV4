/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : IntComMsg.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>
#include <v2/vector/VectorFuncGeneralAuth.h>

class ProtMsg;
class ComDeviceManager;
class ComLinkManager;
class ComDevice;

// -- Qt includes
#include <QObject>
#include <QTimer>
#include <qqml.h>

/**
 * @brief The IntComMsg class
 *
 * Abstract class to create an interface between a Model and many Components.
 * Vector should inherit from this class to communicate with the hardware.
 *
 */


class IntComMsg : public QObject, public Loggable, public XmlProject  {
    Q_OBJECT

    Q_PROPERTY(quint32 prodId READ getProdId WRITE setProdId NOTIFY prodIdChanged)
    Q_PROPERTY(quint32 deviceAd READ getDeviceAd NOTIFY deviceAdChanged)

    QML_ELEMENT

public :

    IntComMsg(QObject *parent);
    virtual ~IntComMsg();

    // -- Configure the system
    Q_INVOKABLE ComLinkManager * getLinkManager() const;
    Q_INVOKABLE ComDeviceManager * getDeviceManager() const;

    // Interface with devices :
    void sendMsg(ProtMsg* p_pMsg, bool p_isEcho = false);  ///< will send the message to the destination device.
    virtual void processReceivedMsg(ProtMsg* p_pMsg) = 0;   ///< this method will be called be all devices with theire received messages. (TODO : private method)

    qint32 getProdId() const;
    qint32 getDeviceAd() const;
    Q_INVOKABLE void setProdId(qint32 p_value);
    Q_INVOKABLE void setProdIdDeviceAd(qint32 p_prodId, qint32 p_deviceAd);
    Q_INVOKABLE void setDeviceAd(qint32 p_value);

    bool isPanelControl(void) const;    ///< return true is this device is a control panel    
    bool isMpc() const;

    virtual void setFuncGeneralParam(const VectorFuncGeneralAuth& p_value);
    const VectorFuncGeneralAuth& getFuncGeneralParam() const;

    void signalEmergencyStopFromDevice(ComDevice* p_pDevice);

    void timeoutAlert(quint32 alertLevel);
    Q_INVOKABLE QString getAlertMsg(quint8 alertLevel) const;
    void stopAlertsTimeouts();

signals :
    void prodIdChanged(quint32);
    void deviceAdChanged(quint32);
    void alertChanged(quint8);

public slots:

    void onTimeoutAlert0();
    void onTimeoutAlert1();
    void onTimeoutAlert2();
    void setAlert(quint32 alertLevel, QString alertMsg);

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Model
    virtual void configureProduct(quint32 p_prodId, quint32 p_deviceAd) = 0;   ///< this method will be called be all devices with theire received messages. (TODO : private method)

    // -- Behaviour
    ComLinkManager * m_pComLinkManager;
    ComDeviceManager * m_pComDeviceManager;

    // -- Properties
//    quint32 m_prodId;      ///< ID of the product, value in PROD_ID
//    quint32 m_deviceAd;      ///< AD of the device
    VectorFuncGeneralAuth m_funcGeneral; ///< contains informations about this vector
    QTimer* m_aAlertsTimeouts[3];

};
