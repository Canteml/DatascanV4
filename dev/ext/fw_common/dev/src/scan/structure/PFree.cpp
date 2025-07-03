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

#include <scan/structure/PFree.h>
#include <scan/zone/Zoning.h>

PFree::PFree() : Piece(Piece::PIE_FREE)
{
    m_pFreeZoning = nullptr;
    init();
}

PFree::PFree(const PFree&) : Piece(Piece::PIE_FREE)
{
    m_pFreeZoning = nullptr;
    init();
}

PFree::~PFree()
{
    m_pFreeZoning = nullptr;
}


// -- Parameters
quint32 PFree::getFreeNbColumn(void) const
{
    return m_freeNbColumn;
}

void PFree::setFreeNbColumn(quint32 p_value)
{
    m_freeNbColumn = p_value;
}

quint32 PFree::getFreeNbRow(void) const
{
    return m_freeNbRow;
}

void PFree::setFreeNbRow(quint32 p_value)
{
    m_freeNbRow = p_value;
}

Zoning *PFree::getFreeZoning() const
{
    return m_pFreeZoning;
}


// -- operator
PFree & PFree::operator= (const PFree & p_value)
{
    if( this != &p_value ) {
        Piece::operator=(p_value);
        m_freeNbColumn = p_value.getFreeNbColumn();
        m_freeNbRow = p_value.getFreeNbRow();
    }
    return (*this);
}

bool PFree::operator== (const PFree & p_value)
{
    return (Piece::operator==(p_value)) &&
        (m_freeNbColumn == p_value.getFreeNbColumn()) &&
        (m_freeNbRow == p_value.getFreeNbRow());
}

Piece *PFree::getCopy() const
{
    return new PFree(*this);
}

// -- XmlProject
bool PFree::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Piece::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(quint32, m_freeNbColumn);
    bRet &= XML_READ_UNIQUE(quint32, m_freeNbRow);
    return bRet;
}

bool PFree::customLoadDefaultParam()
{
    bool bRet = Piece::customLoadDefaultParam();
    init();
    return bRet;
}

bool PFree::customApplyParam()
{
    bool bRet = Piece::customApplyParam();

    if( m_lZoning.isEmpty() ) {
        // make a default zoning
        m_pFreeZoning = new Zoning(Zoning::TYPE_MAT_MEP);
        m_lZoning.append(m_pFreeZoning);
    } else {
        m_pFreeZoning = m_lZoning.first();
    }

    return bRet;
}

bool PFree::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Piece::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_freeNbColumn);
    bRet &= XML_WRITE_UNIQUE(m_freeNbRow);
    return bRet;
}


// -- Behaviour
void PFree::init()
{
    // init Parameters
    m_freeNbColumn = 1;
    m_freeNbRow = 1;

    // zoning
    deleteZoning();
    m_pFreeZoning = new Zoning(Zoning::TYPE_MAT_MEP);
    m_lZoning.append(m_pFreeZoning);
}

void PFree::deleteZoning()
{
    Piece::deleteZoning();
    m_pFreeZoning = nullptr;
}

