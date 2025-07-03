/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XmlSettings.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettings.h>
#include <log/Loggable.h>


// ----------------------------------------------------------------------------
// -- Class XmlSettingsLoggable

/**
 * @brief The XmlSettingsLoggable class
 *
 * Convenient Class to inherit from XmlSettings and Loggable
 *
 */
class XmlSettingsLoggable : public Loggable, public XmlSettings
{
public:
    explicit XmlSettingsLoggable(const QString & p_name);
    virtual ~XmlSettingsLoggable();

    void setSettingsLoggableName(const QString & p_name);
};


