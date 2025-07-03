/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ZoneCell.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/zone/ZoneCell.h>

// ----------------------------------------------------------------------------
// -- Class ZoneCell


ZoneCell::ZoneCell() : XmlProjectLoggable("ZoneCell")
{
    m_value = 0;
}

ZoneCell::~ZoneCell()
{
    m_lLocationMeasure.clear();
}

void ZoneCell::computeValueFromMeasures(void)
{
    // \todo
}

void ZoneCell::addMeasure(LocationMeasure * p_measure)
{
    m_lLocationMeasure.append(p_measure);
    computeValueFromMeasures();
}

qreal ZoneCell::getValue(void) const
{
    return m_value;
}

void ZoneCell::setValue(qreal p_value)
{
    if( m_value != p_value ) {
        m_value = p_value;
        emit evValueChanged();
    }
}

// -- XmlProject
bool ZoneCell::customLoadParam(QDomElement * /*p_pParent*/)
{
    // \todo
    return true;
}

bool ZoneCell::customLoadDefaultParam()
{
    // \todo
    return true;
}

bool ZoneCell::customApplyParam()
{
    // \todo
    return true;
}

bool ZoneCell::customSaveParam(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{
    // \todo
    return true;
}
