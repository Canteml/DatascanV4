/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : PTankRoof.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/PTankRoof.h>

// -- Qt includes
PTankRoof::PTankRoof() : Piece(PIE_TANK_ROOF)
{
    init();
}

PTankRoof::PTankRoof(const PTankRoof & p_value) : Piece(PIE_TANK_ROOF)
{
    init();
    (*this) = p_value;
}

void PTankRoof::init()
{
    // -- Parameters
    m_roofParamNbRayonMode = RAYON_FREE;
    m_roofParamNbRayonFree = 0;
    m_roofParamMeshMode = MESH_FREE;
    m_roofParamMeshFreeNbMesure = 0;
    m_roofParamMeshFreeSpacingCm = 0;

    // -- Properties (computed) :
    m_roofNbRayon = 0;
    m_roofRayonSpacingCm = 0;
    m_roofMeshNbMesure = 0;
    m_roofMeshSpacingCm = 0;
}

PTankRoof::~PTankRoof()
{

}


// -- Parameters
quint32 PTankRoof::getRoofParamNbRayonMode(void) const
{
    return m_roofParamNbRayonMode;
}

void PTankRoof::setRoofParamNbRayonMode(quint32 p_value)
{
    m_roofParamNbRayonMode = p_value;
}

quint32 PTankRoof::getRoofParamNbRayonFree(void) const
{
    return m_roofParamNbRayonFree;
}

void PTankRoof::setRoofParamNbRayonFree(quint32 p_value)
{
    m_roofParamNbRayonFree = p_value;
}

quint32 PTankRoof::getRoofParamMeshMode(void) const
{
    return m_roofParamMeshMode;
}

void PTankRoof::setRoofParamMeshMode(quint32 p_value)
{
    m_roofParamMeshMode = p_value;
}

quint32 PTankRoof::getRoofParamMeshFreeNbMesure(void) const
{
    return m_roofParamMeshFreeNbMesure;
}

void PTankRoof::setRoofParamMeshFreeNbMesure(quint32 p_value)
{
    m_roofParamMeshFreeNbMesure = p_value;
}

quint32 PTankRoof::getRoofParamMeshFreeSpacingCm(void) const
{
    return m_roofParamMeshFreeSpacingCm;
}

void PTankRoof::setRoofParamMeshFreeSpacingCm(quint32 p_value)
{
    m_roofParamMeshFreeSpacingCm = p_value;
}


// -- Properties
void PTankRoof::computePropertiesFromParam(void)
{
    // \todo

}

quint32 PTankRoof::getRoofNbRayon(void) const
{
    return m_roofNbRayon;
}

qreal PTankRoof::getRoofRayonSpacingCm(void) const
{
    return m_roofRayonSpacingCm;
}

quint32 PTankRoof::getRoofMeshNbMesure(void) const
{
    return m_roofMeshNbMesure;
}

qreal PTankRoof::getRoofMeshSpacingCm(void) const
{
    return m_roofMeshSpacingCm;
}


// -- XmlProject
bool PTankRoof::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Piece::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(quint32, m_roofParamNbRayonMode);
    bRet &= XML_READ_UNIQUE(quint32, m_roofParamNbRayonFree);
    bRet &= XML_READ_UNIQUE(quint32, m_roofParamMeshMode);
    bRet &= XML_READ_UNIQUE(quint32, m_roofParamMeshFreeNbMesure);
    bRet &= XML_READ_UNIQUE(quint32, m_roofParamMeshFreeSpacingCm);
    return bRet;
}

bool PTankRoof::customLoadDefaultParam()
{
    bool bRet = Piece::customLoadDefaultParam();

    // -- Parameters
    m_roofParamNbRayonMode = RAYON_FREE;
    m_roofParamNbRayonFree = 0;
    m_roofParamMeshMode = MESH_FREE;
    m_roofParamMeshFreeNbMesure = 0;
    m_roofParamMeshFreeSpacingCm = 0;

    // -- Properties (computed) :
    m_roofNbRayon = 0;
    m_roofRayonSpacingCm = 0;
    m_roofMeshNbMesure = 0;
    m_roofMeshSpacingCm = 0;

    return bRet;
}

bool PTankRoof::customApplyParam()
{
    bool bRet = Piece::customApplyParam();
    computePropertiesFromParam();
    return bRet;
}

bool PTankRoof::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Piece::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_roofParamNbRayonMode);
    bRet &= XML_WRITE_UNIQUE(m_roofParamNbRayonFree);
    bRet &= XML_WRITE_UNIQUE(m_roofParamMeshMode);
    bRet &= XML_WRITE_UNIQUE(m_roofParamMeshFreeNbMesure);
    bRet &= XML_WRITE_UNIQUE(m_roofParamMeshFreeSpacingCm);
    return bRet;
}


// -- operator
PTankRoof & PTankRoof::operator= (const PTankRoof & p_value)
{
    if( this != &p_value) {
        m_roofParamNbRayonMode = p_value.getRoofParamNbRayonMode();
        m_roofParamNbRayonFree = p_value.getRoofParamNbRayonFree();
        m_roofParamMeshMode = p_value.getRoofParamMeshMode();
        m_roofParamMeshFreeNbMesure = p_value.getRoofParamMeshFreeNbMesure();
        m_roofParamMeshFreeSpacingCm = p_value.getRoofParamMeshFreeSpacingCm();

        computePropertiesFromParam();
    }

    return (*this);
}

bool PTankRoof::operator== (const PTankRoof & p_value)
{
    return (m_roofParamNbRayonMode == p_value.getRoofParamNbRayonMode()) &&
            (m_roofParamNbRayonFree == p_value.getRoofParamNbRayonFree()) &&
            (m_roofParamMeshMode == p_value.getRoofParamMeshMode()) &&
            (m_roofParamMeshFreeNbMesure == p_value.getRoofParamMeshFreeNbMesure()) &&
            (m_roofParamMeshFreeSpacingCm == p_value.getRoofParamMeshFreeSpacingCm());

}

Piece *PTankRoof::getCopy() const
{
    return new PTankRoof(*this);
}
