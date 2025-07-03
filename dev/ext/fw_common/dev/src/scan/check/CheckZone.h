/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckZone.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>


// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class CheckZone

/**
 * @brief the CheckZone class
 *
 */
class CheckZone : public QObject, public XmlProjectLoggable
{
public :
    explicit CheckZone();
    virtual ~CheckZone();


    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

};


