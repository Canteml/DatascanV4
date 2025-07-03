/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Calibrate.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/type/ProjectDate.h>

// ----------------------------------------------------------------------------
// -- Class Calibrate

/**
 * @brief the Calibrate class
 *
 * Just a base to get a date and convenient methods to set them.
 *
 */
 
class Calibrate : public ProjectDate
{

public :
    explicit Calibrate(const QString & p_className = "Calibrate");
    explicit Calibrate(const Calibrate & p_value);
    virtual ~Calibrate();   
};
