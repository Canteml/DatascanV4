/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkSerial.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/com/GenComLink.h>

// --- Qt includes
#include <QSerialPort>
#include <qqml.h>

// ----------------------------------------------------------------------------
// -- ComLinkSerialConfig

/**
 * @brief The ComLinkSerialConfig class
 *
 * Stock parameters of an object ComLinkSerial
 *
 */
class ComLinkSerialConfig : public QObject, public XmlSettingsLoggable {
    Q_OBJECT
    Q_PROPERTY(quint32 portNumber READ getPortNumber WRITE setPortNumber NOTIFY portNumberChanged)
    Q_PROPERTY(quint32 baudrate READ getBaudrate WRITE setBaudrate NOTIFY baudrateChanged)

    QML_ELEMENT

public :
    ComLinkSerialConfig();
    ~ComLinkSerialConfig();

    void useDefaultConfig();

    void setConfig(const ComLinkSerialConfig& p_valueIn);
    void getConfig(ComLinkSerialConfig& p_valueInOut) const;

    static const qint32 SERIAL_BAUDRATE = 921600 ;

// -- Properties
public slots :
    quint32 getPortNumber() const;
    void setPortNumber(quint32 p_value);
    quint32 getBaudrate() const;
    void setBaudrate(quint32 p_value);

signals :
    void portNumberChanged();
    void baudrateChanged();

protected :
// -- XmlSettings behaviour
    bool customLoadSettings(QDomElement * p_pParent) override;
    bool customLoadDefaultSettings() override;
    bool customApplySettings() override;
    bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :
    // -- Parameters
    quint32 m_portNumber;
    quint32 m_baudrate;
};


// ----------------------------------------------------------------------------
// -- ComLinkSerial

/**
 * @brief The ComLinkSerial class
 */
class ComLinkSerial : public GenComLink {
    Q_OBJECT

    QML_ELEMENT

public :
    ComLinkSerial(IntComMsg* p_pIntComMsg);
    ~ComLinkSerial();

    void setConfig(const ComLinkSerialConfig& p_valueIn);
    void setConfig(quint32 p_port, quint32 p_bauds);
    void getConfig(ComLinkSerialConfig& p_valueInOut) const;

    quint32 getPortNumber() const;
    quint32 getBaudrate() const;

    virtual QString getConfigHelp() const override;  ///< return a string with the link configuration (COM1, 9600 bauds ...)

    void setLinkSerialConfig(ComLinkSerialConfig * p_conf) override;

public slots :
    virtual quint32 getType() const override;    ///< value in LINK_TYPE
    virtual ComLinkSerial * getLinkSerial() override;

private slots :
    void onErrorOccured(QSerialPort::SerialPortError);


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

private :
    ComLinkSerialConfig m_config;
};


