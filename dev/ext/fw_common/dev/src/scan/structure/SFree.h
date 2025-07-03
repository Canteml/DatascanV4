/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : SFree.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Structure.h>
class PFree;

// -- Qt includes


/**
 * @brief the PFree class
 *
 */
class SFree : public Structure
{
    Q_OBJECT
public :

    explicit SFree();
    explicit SFree(const SFree & p_value);
    virtual ~SFree();

    // -- Behaviour
    PFree * getPieceFree() const;
    bool generateZoning();

    // -- operator
    SFree & operator= (const SFree & p_value);
    bool operator== (const SFree & p_value);
    virtual Structure * getCopy() const override;

protected :

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    void init();
    virtual void deletePieces() override;

    PFree * m_pFreePiece;   ///< std piece
};

