/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : type/SystemTransformation.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>

// -- Qt includes
#include <QPointF>

// ----------------------------------------------------------------------------
// -- Class SystemTransformation

/**
* @brief The systemTransformation struct
*
*  r and t denotes the rotational and translational mapping from parent system to a new system
*
*/
class SystemTransformation : public XmlProjectLoggable
{
public :

    explicit SystemTransformation();
    SystemTransformation(const SystemTransformation& p_value);

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    SystemTransformation & operator= (const SystemTransformation & p_value);
    bool operator== (const SystemTransformation & p_value);

    QPointF m_translate;
    qreal m_rotate;
};

