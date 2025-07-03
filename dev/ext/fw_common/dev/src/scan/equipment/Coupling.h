/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Coupling.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>

// -- Qt includes

/**
 * @brief the Coupling class
 *
 * Coupling is a material applied between the probe and the piece to improve the signal of the measure.
 *
 */
class Coupling : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    Coupling();
    Coupling(const Coupling & p_value);
    ~Coupling();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Param
    QString getParamType() const;
    void setParamType(const QString & p_type);

    QString getParamBrand(void) const;
    void setParamBrand(const QString & p_brand);

    QString getParamExpirationDate(void) const;
    void setParamExpirationDate(const QString & p_expirationDate);

    Coupling& operator=(const Coupling& p_value);
    bool operator==(const Coupling& p_value);

private :

    // param
    QString m_paramType;     ///< EAU / GEL ...
    QString m_paramBrand;
    QString m_paramExpirationDate;
};

