/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XXX.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/SFree.h>
#include <scan/structure/PFree.h>
#include <scan/structure/Piece.h>
#include <scan/zone/Zoning.h>
#include <scan/zone/Zone.h>

// -- Qt includes

SFree::SFree() : Structure(Structure::STR_FREE)
{
    m_pFreePiece = nullptr;
    init();
}

SFree::SFree(const SFree &p_value) : Structure(Structure::STR_FREE)
{
    init();
    (*this) = p_value;
}

SFree::~SFree()
{
    // piece is deleted by class Structure
    m_pFreePiece = nullptr;
}

// -- Behaviour
PFree *SFree::getPieceFree() const
{
    return m_pFreePiece;
}

bool SFree::generateZoning()
{

    return true;
}

// -- XmlProject
bool SFree::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Structure::customLoadParam(p_pParent);
    return bRet;
}

bool SFree::customLoadDefaultParam()
{
    bool bRet = Structure::customLoadDefaultParam();
    init();
    return bRet;
}

bool SFree::customApplyParam()
{
    bool bRet = Structure::customApplyParam();
    // -- set pointer to the piece
    if( m_lPieces.size() == 1 ) {
        m_pFreePiece = static_cast<PFree *>(m_lPieces.at(0));
    } else {
        fLogWarning("bad list => use default piece");
        init();
    }
    return bRet;
}

bool SFree::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Structure::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}

// -- operator
SFree & SFree::operator= (const SFree & p_value)
{
    if( this != &p_value ) {
        Structure::operator=(p_value);
    }
    return (*this);
}

bool SFree::operator== (const SFree & p_value)
{
    return (Structure::operator==(p_value));
}

Structure *SFree::getCopy() const
{
    return new SFree(*this);
}

void SFree::init()
{
    deletePieces();

    // instanciate the free piece
    m_pFreePiece = new PFree();
    m_lPieces.append(m_pFreePiece);



}

void SFree::deletePieces()
{
    // We consider that the first element of the list is the Piece => nothing to dereference
    Structure::deletePieces();
    m_pFreePiece = nullptr;
}
