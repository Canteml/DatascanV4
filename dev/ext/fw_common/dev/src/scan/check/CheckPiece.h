/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckPiece.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class CheckZoning;
class Report;


// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class CheckPiece

/**
 * @brief the CheckPiece class
 *
 */
class CheckPiece : public QObject, public XmlProjectLoggable
{
public :
    explicit CheckPiece();
    virtual ~CheckPiece();


    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    // -- Properties
    quint32 m_pieceId;
    Report * m_pReport; ///< report for this piece of the structure

};


