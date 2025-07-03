/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckZoning.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/check/CheckZoning.h>

// ----------------------------------------------------------------------------
// -- Class CheckZoning


CheckZoning::CheckZoning() : XmlProjectLoggable("CheckZoning")
{
//    protected :

//    Zoning * m_pZoningReference;
//    QList<CheckLocation *> m_lCheckLocation;
//    QVector<QVector<CheckZone *>> m_matCheckZone;
}

CheckZoning::~CheckZoning()
{

}


void CheckZoning::setZoneMatrixSize(quint32 /*p_x*/, quint32 /*p_y*/)
{

}

quint32 CheckZoning::getZoneMatrixSizeX() const
{
    return 0;
}

quint32 CheckZoning::getZoneMatrixSizeY() const
{
    return 0;
}

CheckZone * CheckZoning::getZoneItem(int /*p_x*/, int /*p_y*/) const
{
    return nullptr;
}

void CheckZoning::removeAllZone()
{

}


quint32 CheckZoning::getLocationListSize() const
{
    return 0;
}

CheckLocation * CheckZoning::getLocation(int /*p_index*/) const
{
    return nullptr;
}

void CheckZoning::addLocation(int /*p_index*/, CheckLocation * /*p_pLocation*/)
{

}

void CheckZoning::removeLocation(int /*p_index*/)
{

}

void CheckZoning::removeAllLocation()
{

}

// -- XmlProject
bool CheckZoning::customLoadParam(QDomElement * /*p_pParent*/)
{
    return true;
}

bool CheckZoning::customLoadDefaultParam()
{
    return true;
}

bool CheckZoning::customApplyParam()
{
    return true;
}

bool CheckZoning::customSaveParam(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{
    return true;
}



