/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpServer.h
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
#include <QTcpServer>

/**
 * @brief The ComLinkTcpServer class
 *
 * This class can't create / configure a TcpSocket. Use ComLinkManager to make TcpSocket.
 *
 */
class ComLinkTcpServer : public GenComLink {
    Q_OBJECT

public :
    ComLinkTcpServer(IntComMsg* p_pIntComMsg);   ///< Socket must be opened and connected
    ~ComLinkTcpServer();

    quint16 getPortNumber() const;

    virtual QString getConfigHelp() const override;  ///< return a string with the link configuration (COM1, 9600 bauds ...)

    bool startServer(quint16 p_port);
    void stopServer();

public slots :
    virtual quint32 getType() const override;           ///< value in LINK_TYPE

protected slots :
    void onTcpSocketCliDisconnected();
    void onNewTcpServerConnection();

protected :
    // -- Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    virtual bool tryOpen() override;

protected :
    // -- vecLink
    QString interfaceName() const override;
    QIODevice * makeInterface() override;
    virtual void removeLink() override;

    // -- confoig
    ComLinkTcpConfig m_comLinkTcpConfig;

    // -- TCP
    QTcpSocket * m_pTcpSocketCli;
    QTcpServer *m_pTcpServer;   ///< Used if Vector id Devices & simu
    quint16 m_tcpServerPort;    ///< Used if Vector id Devices & simu
};
