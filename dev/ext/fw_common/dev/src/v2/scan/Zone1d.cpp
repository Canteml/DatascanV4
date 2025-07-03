/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Zone1d.cpp
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/scan/Zone1d.h>

// -- Qt Includes
Zone1d::Zone1d() : Loggable("Zone1d")
{
    m_originXMm = 0;
    m_sizeXMm = 0;
}

Zone1d::~Zone1d()
{

}

bool Zone1d::isSizeDefined() const
{
    return 0 != m_sizeXMm;
}

qreal Zone1d::getOriginXMm() const
{
    return m_originXMm;
}

void Zone1d::setOriginXMm( qreal p_value)
{
    if( m_originXMm != p_value ) {
        m_originXMm = p_value;
        emit evChanged();
    }
}

qreal Zone1d::getSizeXMm() const
{
    return m_sizeXMm;
}

void Zone1d::setSizeXMm(qreal p_value)
{
    if( m_sizeXMm != p_value ) {
        m_sizeXMm = p_value;
        emit evChanged();
    }
}
