/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CheckPiece.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/check/CheckPiece.h>
#include <scan/type/Report.h>
#include <scan/check/CheckZoning.h>

CheckPiece::CheckPiece() : XmlProjectLoggable("CheckPiece")
{
    m_pReport = new Report();
    m_pieceId = 0;
}

CheckPiece::~CheckPiece()
{
    delete m_pReport;
    m_pReport = nullptr;
}

bool CheckPiece::customLoadParam(QDomElement * /*p_pParent*/)
{
    return true;
}

bool CheckPiece::customLoadDefaultParam()
{
    return true;
}

bool CheckPiece::customApplyParam()
{
    return true;
}

bool CheckPiece::customSaveParam(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{
    return true;
}
