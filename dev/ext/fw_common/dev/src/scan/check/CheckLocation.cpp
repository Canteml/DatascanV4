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
#include <scan/check/CheckLocation.h>
#include <scan/zone/LocationMeasure.h>


CheckLocation::CheckLocation() : XmlProjectLoggable("CheckLocation")
{

}

CheckLocation::~CheckLocation()
{
    clearMeasure();
}

void CheckLocation::clearMeasure()
{
    for(auto i : qAsConst(m_lMeasure)) {
        delete i;
    }
    m_lMeasure.clear();
}

void CheckLocation::addMeasure(LocationMeasure * m_pMeasure)
{
    m_lMeasure.append(m_pMeasure);
}

void CheckLocation::makeMeasure(QPointF p_position, QVector<qint16> * p_samples)
{
    LocationMeasure * m = new LocationMeasure();
    m->setTimeToNow();
    m->setPosition(p_position);
    m->setSamples(p_samples);
    m->computeValueFromSample();
    addMeasure(m);
}

void CheckLocation::makeMeasure(QPointF p_position, QVector<qint16> * p_samples, qreal p_value)
{
    LocationMeasure * m = new LocationMeasure();
    m->setTimeToNow();
    m->setPosition(p_position);
    m->setSamples(p_samples);
    m->setValue(p_value);
    addMeasure(m);
}

// -- XmlProject
bool CheckLocation::customLoadParam(QDomElement * /*p_pParent*/)
{
    // \todo
    return true;
}

bool CheckLocation::customLoadDefaultParam()
{
    // \todo
    return true;
}

bool CheckLocation::customApplyParam()
{
    // \todo
    return true;
}

bool CheckLocation::customSaveParam(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{
    // \todo
    return true;
}
