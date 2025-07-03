/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpCli.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/com/GenComLink.h>
#include <v2/com/ComLinkTcpConfig.h>

// --- Qt includes
#include <QTcpSocket>
#include <qqml.h>


/**
 * @brief The ComLinkTcpCli class
 *
 * This class can't create / configure a TcpSocket. Use ComLinkManager to make TcpSocket.
 *
 */
class ComLinkTcpCli : public GenComLink {
    Q_OBJECT

public :
    ComLinkTcpCli(IntComMsg* p_pIntComMsg);   ///< Socket must be opened and connected
    ~ComLinkTcpCli();

    void getConfig(ComLinkTcpConfig &p_valueInOut) const;

    quint32 getPortNumber() const;

    virtual void setLinkTcpConfig(ComLinkTcpConfig * p_conf) override;
    virtual QString getConfigHelp() const override;  ///< return a string with the link configuration (COM1, 9600 bauds ...)

    bool tryConnect();///< used by discover mode

public slots :
    virtual quint32 getType() const override;           ///< value in LINK_TYPE
    virtual ComLinkTcpCli * getLinkTcpCli() override;

protected slots :
    void onSocketClosed();

protected :
    // -- Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    virtual bool tryOpen() override;
    virtual void removeLink() override;

protected :
    // -- vecLink
    QString interfaceName() const override;
    QIODevice * makeInterface() override;

    // -- config
    ComLinkTcpConfig m_comLinkTcpConfig;

    // -- TCP
    QTcpSocket * m_pTcpSocket;
};
