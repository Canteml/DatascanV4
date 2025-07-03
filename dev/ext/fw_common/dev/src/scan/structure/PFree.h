/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : PFree.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Piece.h>
class Zoning;

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class PFree

/**
 * @brief the PTankRoof class
 *
 */
class PFree : public Piece
{
    Q_OBJECT
public :
    PFree();
    PFree(const PFree&);
    ~PFree();

    // -- Parameters
    quint32 getFreeNbColumn(void) const;
    void setFreeNbColumn(quint32 p_value);
    quint32 getFreeNbRow(void) const;
    void setFreeNbRow(quint32 p_value);

    // -- Behaviour
    Zoning *getFreeZoning() const;

    // -- operator
    PFree & operator= (const PFree & p_value);
    bool operator== (const PFree & p_value);
    virtual Piece *getCopy() const override;

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    // -- Behaviour
    void init();
    void deleteZoning() override;
    Zoning * m_pFreeZoning; ///< only one zoning for free piece

    // -- Parameters
    quint32 m_freeOperatingMode;
    quint32 m_freeNbColumn;
    quint32 m_freeNbRow;
};
