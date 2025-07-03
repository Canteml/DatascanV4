/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : DomDevice.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
#include <v2/prot/ProtMsg.h>
#include <v2/vector/VectorFuncGeneralAuth.h>
class GenComLink;
class IntComMsg;

// -- Qt includes
#include <QObject>
#include <qqml.h>

/**
 * @brief The ComDevice class
 *
 */
class ComDevice : public QObject, public XmlProjectLoggable {
    Q_OBJECT

    Q_PROPERTY(quint32 comState READ getComState NOTIFY comStateChanged)

    // -- Function General
    Q_PROPERTY(qint8 paramDProdId READ getParamDProdId WRITE setParamDProdId NOTIFY paramDProdIdChanged)
    Q_PROPERTY(qint8 paramDDeviceAd READ getParamDDeviceAd WRITE setParamDDeviceAd NOTIFY paramDDeviceAdChanged)
    Q_PROPERTY(QString paramDProdUid READ getParamDProdUid WRITE setParamDProdUid NOTIFY paramDProdUidChanged)
    Q_PROPERTY(QString paramDVerHw READ getParamDVerHw WRITE setParamDVerHw NOTIFY paramDVerHwChanged)
    Q_PROPERTY(QString paramDVerSw READ getParamDVerSw WRITE setParamDVerSw NOTIFY paramDVerSwChanged)
    Q_PROPERTY(qint64 paramDDateBuild READ getParamDDateBuild WRITE setParamDDateBuild NOTIFY paramDDateBuildChanged)
    Q_PROPERTY(qint64 paramDDateRev READ getParamDDateRev WRITE setParamDDateRev NOTIFY paramDDateRevChanged)

public :

    enum COM_STATE {
        COM_NOT_LINKED = 0,
        COM_LINKED_NOT_CONNECTED,
        COM_LINKED_CONNECTED
    };

    //ComDevice(IntComMsg * p_pIntComMsg, quint32 p_prodId, quint32 p_deviceAd);   ///< Device made by simu
    //ComDevice(GenComLink * p_pComLink, quint32 p_prodId, quint32 p_deviceAd);    ///< Device discovered (by MPC), with a connected link
    ComDevice(IntComMsg * p_pIntComMsg, GenComLink * p_pComLink, const VectorFuncGeneralAuth& p_param);    ///< Device discovered (by MPC), with a connected link

    ~ComDevice();

    // -- Function General
    qint32 getParamDProdId() const;
    void setParamDProdId(qint32 p_value, bool b_syncProt = true);

    qint32 getParamDDeviceAd() const;
    void setParamDDeviceAd(qint32 p_value, bool b_syncProt = true);

    QString getParamDProdUid() const;
    void setParamDProdUid(const QString & p_value, bool b_syncProt = true);

    QString getParamDVerHw() const;
    void setParamDVerHw(const QString & p_value, bool b_syncProt = true);

    QString getParamDVerSw() const;
    void setParamDVerSw(const QString & p_value, bool b_syncProt = true);

    qint64 getParamDDateBuild() const;
    void setParamDDateBuild(qint64 p_value, bool b_syncProt = true);

    qint64 getParamDDateRev() const;
    void setParamDDateRev(qint64 p_value, bool b_syncProt = true);

    // -- Properties    
    void setFunctions(const QList<quint32> & p_list, bool b_syncProt = true);
    const QList<quint32> & getFunctions(void);
    void addFunctions(const QList<quint32> & p_list);
    void resetFunctions();

    // -- Behaviour
    void isUsed(void) const;    ///< return true if the IntComMsg is set
    quint32 getComState() const;

    QString getDeviceName() const;
    void setDeviceName(const QString& p_value);

    // -- Communications
    void setLink(GenComLink * p_pComLink);
    GenComLink* getLink(void) const;

    void sendMsg(ProtMsg * p_pMsg);

    void emergencyStop();

private slots :
    void onNewLinkMsg();
    void onLinkStateChanged(GenComLink*, quint32);

signals :
    void evLinkChanged(GenComLink * p_pComLink);
    void comStateChanged(quint32 state);

    // -- Function General
    void paramDProdIdChanged(qint8);
    void paramDDeviceAdChanged(qint8);
    void paramDProdUidChanged(QString);
    void paramDVerHwChanged(QString);
    void paramDVerSwChanged(QString);
    void paramDDateBuildChanged(qint64);
    void paramDDateRevChanged(qint64);
    void paramLFunctionsChanged(const QList<quint32>);

protected :
    void setComState(quint32 p_value);
    ProtMsg * newMsgSetParamGeneral(qint32 p_param);

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    // -- Properties (used to identiy this device, get with a message : MSG_CMD_WHO_ARE_YOU)
    GenComLink * m_pComLink;                ///< reference to the link
    IntComMsg* m_pIntComMsg;                ///< Vector where incoming messages are treated (dispatched by functions). set only if this Device is used in the system

    QString m_deviceName;   ///< used to match with a link name. Defined in same time that m_deviceAd
    quint32 m_comState;     ///< value in COM_STATE

    // Parameters (Function General)
    VectorFuncGeneralAuth m_paramFuncGeneralAuth;

    QMutex * m_mLink;   ///< protect link access
};
