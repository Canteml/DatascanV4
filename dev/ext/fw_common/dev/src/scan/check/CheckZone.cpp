/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckZone.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/check/CheckZone.h>

// ----------------------------------------------------------------------------
// -- Class CheckZone


CheckZone::CheckZone() : XmlProjectLoggable("CheckZone")
{

}

CheckZone::~CheckZone()
{

}

// -- XmlProject
bool CheckZone::customLoadParam(QDomElement * /*p_pParent*/)
{
    return true;
}

bool CheckZone::customLoadDefaultParam()
{
    return true;
}

bool CheckZone::customApplyParam()
{
    return true;
}

bool CheckZone::customSaveParam(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{
    return true;
}
