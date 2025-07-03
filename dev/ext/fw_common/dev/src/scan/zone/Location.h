/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Location.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
#include <scan/type/SystemTransformation.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class Location

class Location : public XmlProjectLoggable
{
public :

    explicit Location();
    Location(const Location& p_value);
    ~Location();

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    quint32 getLocationId(void) const;
    void setLocationId(quint32 p_value);

    // -- Properties
    QString getLocationName(void) const;
    void setLocationName(const QString & p_value);
    SystemTransformation getLocationTransformation(void) const;
    void setLocationTransformation(const SystemTransformation & p_value);


    // -- operator
    Location & operator= (const Location & p_value);
    bool operator== (const Location & p_value);

private :

    // -- Behaviour
    quint32 m_locationId;

    // -- Properties
    QString m_locationName;
    SystemTransformation m_locationTransformation;  ///< translation and rotation from piece system (parent), origin of the vector

};
