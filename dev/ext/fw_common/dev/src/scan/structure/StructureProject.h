/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Environment.h
# Author      : Thierry Lechevallier
# Date        : 28/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Structure.h>

// -- Qt includes

/**
 * @brief the StructureProject class
 *
 * Convenient class used to load a structure (useless to save it).
 * Container of class Structure, instanciate a structure function to its type.
 *
 */
class StructureProject : public XmlProjectLoggable
{
public :

    explicit StructureProject();
    ~StructureProject();

    Structure* takeStructure();

    static Structure* instanciateStructure(quint32 p_type);     ///< value in StructureType

protected :
    // -- XmlSettings
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    Structure * m_pStructure;
};

