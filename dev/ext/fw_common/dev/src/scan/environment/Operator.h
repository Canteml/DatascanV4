/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Operator.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>

// -- Qt includes


/**
 * @brief the Operator class
 *
 * People that execute the exam
 *
 */
class Operator : public QObject, public XmlProjectLoggable
{
public :
    explicit Operator();
    explicit Operator(const Operator & p_value);
    ~Operator();

    // -- Properties
    QString getFirstName() const;
    void setFirstName(const QString & p_value);
    QString getLastName(void) const;
    void setLastName(const QString & p_value);
    QString getCardNumber(void) const;
    void setCardNumber(const QString & p_value);

    // -- operators
    bool operator==(const Operator & p_value);
    Operator & operator=(const Operator & p_value);

protected :
    // -- XmlSettings
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :

    // -- Properties
    QString m_firstName;
    QString m_lastName;
    QString m_cardNumber;
};

