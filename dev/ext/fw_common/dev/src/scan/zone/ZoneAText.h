/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XXX.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/zone/ZoneAnnotation.h>

// -- Qt includes


/**
 * @brief the ZoneAText class
 *
 */
class ZoneAText : public ZoneAnnotation
{
public :
    ZoneAText();
    ~ZoneAText();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Properties
    QString getText(void) const;
    void setText(const QString& p_value);

protected :
    // -- Properties
    QString m_text;

};

