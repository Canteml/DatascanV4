/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : XXX.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Piece.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class PTankRoof

/**
 * @brief the PTankRoof class
 *
 */
class PTankRoof : public Piece
{
    Q_OBJECT
public :
    PTankRoof();
    PTankRoof(const PTankRoof&);
    ~PTankRoof();

    // -- Parameters
    quint32 getRoofParamNbRayonMode(void) const;
    void setRoofParamNbRayonMode(quint32 p_value);
    quint32 getRoofParamNbRayonFree(void) const;
    void setRoofParamNbRayonFree(quint32 p_value);
    quint32 getRoofParamMeshMode(void) const;
    void setRoofParamMeshMode(quint32 p_value);
    quint32 getRoofParamMeshFreeNbMesure(void) const;
    void setRoofParamMeshFreeNbMesure(quint32 p_value);
    quint32 getRoofParamMeshFreeSpacingCm(void) const;
    void setRoofParamMeshFreeSpacingCm(quint32 p_value);

    // -- Properties

    enum RoofParamRayonMode {
        RAYON_FREE,
        RAYON_CARDINAL,
        RAYON_DT94
    };

    enum RoofParamMeshMode {
        MESH_FREE,
        MESH_SPACING
    };

    void computePropertiesFromParam(void);
    quint32 getRoofNbRayon(void) const;
    qreal getRoofRayonSpacingCm(void) const;
    quint32 getRoofMeshNbMesure(void) const;
    qreal getRoofMeshSpacingCm(void) const;

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    PTankRoof & operator= (const PTankRoof & p_value);
    bool operator== (const PTankRoof & p_value);
    virtual Piece *getCopy() const override;

protected :

    // -- Behaviour
    void init();

    // -- Parameters
    quint32 m_roofParamNbRayonMode;  ///< 0 = free, 1 = Cardinal (E/O/N/S), 2 = DT94
    quint32 m_roofParamNbRayonFree;  ///< used if m_roobNbRayonMode = 0
    quint32 m_roofParamMeshMode; ///< 0 = free nb mesures, 1 = free spacing
    quint32 m_roofParamMeshFreeNbMesure;
    quint32 m_roofParamMeshFreeSpacingCm;

    // -- Properties (computed) :
    quint32 m_roofNbRayon;
    qreal m_roofRayonSpacingCm;
    quint32 m_roofMeshNbMesure;
    qreal m_roofMeshSpacingCm;
};
