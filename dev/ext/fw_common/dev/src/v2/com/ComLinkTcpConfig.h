/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpConfig.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes

#include <v2/com/GenComLink.h>

// --- Qt includes
#include <QTcpSocket>
#include <qqml.h>

/**
 * @brief The ComLinkTcpCliConfig class
 *
 * Stock parameters of an object ComLinkTcp (Cli / Server)
 *
 */
class ComLinkTcpConfig : public QObject, public XmlSettingsLoggable {
    Q_OBJECT
    Q_PROPERTY(quint32 portNumber READ getPortNumber WRITE setPortNumber NOTIFY portNumberChanged)    
    Q_PROPERTY(QString hostAddress READ getHostAddress WRITE setHostAddress NOTIFY hostAddressChanged)

    QML_ELEMENT

public :
    ComLinkTcpConfig();
    ~ComLinkTcpConfig();

    void useDefaultConfig();

    void setConfig(const ComLinkTcpConfig& p_valueIn);
    void getConfig(ComLinkTcpConfig& p_valueInOut) const;

    bool isHostAddressDefined() const;

    static const qint32 TCP_PORT_DEFAULT = 2110;
    static const QString TCP_BASE_IP;

// -- Properties
public slots :
    quint32 getPortNumber() const;
    void setPortNumber(quint32 p_value);
    QString getHostAddress() const;
    void setHostAddress(QString p_value);

signals :
    void portNumberChanged(quint32);
    void hostAddressChanged(QString);

protected :
// -- XmlSettings behaviour
    bool customLoadSettings(QDomElement * p_pParent) override;
    bool customLoadDefaultSettings() override;
    bool customApplySettings() override;
    bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    // -- Parameters
    quint32 m_portNumber;
    QString m_hostAddress;
};
