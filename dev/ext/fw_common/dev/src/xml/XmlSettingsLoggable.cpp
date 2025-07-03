/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlSettings.cpp
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>

// -- Qt includes

XmlSettingsLoggable::XmlSettingsLoggable(const QString & p_name) : Loggable(p_name), XmlSettings(p_name)
{

}

XmlSettingsLoggable::~XmlSettingsLoggable()
{

}

void XmlSettingsLoggable::setSettingsLoggableName(const QString &p_name)
{
    setLogName(p_name);
    setXmlSettingsName(p_name);
}

