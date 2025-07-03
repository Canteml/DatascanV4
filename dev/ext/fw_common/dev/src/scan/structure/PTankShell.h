/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : PTankShell.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Piece.h>
class STankFeature;

// -- Qt includes




// ----------------------------------------------------------------------------
// -- Class Virole

/**
 * @brief the Virole class
 *
 */
class Virole : public XmlProjectLoggable
{
public :

    Virole();
    ~Virole();

    // -- Parameters
    QString getViroleName() const;
    void setViroleName(const QString & p_value);
    qreal getViroleOriginalThikness(void) const;
    void setViroleOriginalThikness(qreal p_value);
    quint32 getViroleNbZone() const;
    void setViroleYNbZone(quint32 p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    Virole & operator= (const Virole & p_value);
    bool operator== (const Virole & p_value);

protected :
    QString m_viroleName;
    qreal m_viroleOriginalThickness;
    quint32 m_viroleNbYZone;            ///< number of zone on Y Axis

};

// ----------------------------------------------------------------------------
// -- Class PTankShell

/**
 * @brief the PTankShell class
 *
 */
class PTankShell : public Piece
{
public :
    PTankShell();
    PTankShell(const PTankShell& p_value);
    ~PTankShell();

    enum TankShellOperatingMode {
        OPERATING_GENERATRIX,
        OPERATING_CIRCULAR
    };

    enum GeneratrixPerimeterMode {
        GENERATRIX_P_FREE,
        GENERATRIX_P_CARDINAL,
        GENERATRIX_P_DT94
    };

    enum GeneratrixHeightMode {
        GENERATRIX_H_FREE,
        GENERATRIX_H_SPACING,
        GENERATRIX_H_VIROLE
    };

    enum CircularHeightMode {
        CIRCULAR_H_FREE,
        CIRCULAR_H_VIROLE
    };

    enum CircularPerimeterMode {
        CIRCULAR_P_FREE,
        CIRCULAR_P_SPACING
    };

    // -- Behaviour
    void setTankFeature(STankFeature * p_pSTankFeature);   

    // -- Parameters
    quint32 getNbOfVirole() const;
    Virole* addVirole(qint32 p_viroleIndex = -1);  ///< if index = -1 : virole added to the end
    void removeVirole(qint32 p_viroleIndex);
    Virole* getVirole(qint32 p_viroleIndex) const;

    void computePropertiesFromParameters(void);

    quint32 getOperatorMode(void) const;
    void setOperatorMode(quint32 p_value);
    quint32 getGeneratrixPerimeterMode(void) const;
    void setGeneratricPerimeterMode(quint32 p_value);
    quint32 getGeneratrixPerimeterNbFree(void) const;
    void setGeneratrixPerimeterNbFree(quint32 p_value);
    quint32 getGeneratrixHeightMode(void) const;
    void setGeneratricHeightMode(quint32 p_value);
    quint32 getGeneratrixHeightNbFree(void) const;
    void setGeneratricHeightNbFree(quint32 p_value);
    quint32 getGeneratrixHeightSpacingCm(void) const;
    void setGeneratricHeightSpacingCm(quint32 p_value);
    quint32 getGeneratrixHeightNbVirole(void) const;
    void setGeneratricHeightNbVirole(quint32 p_value);
    quint32 getGeneratrixHeightNbMeasureByVirole(void) const;
    void setGeneratricHeightNbMeasureByVirole(quint32 p_value);
    bool getGeneratrixHeightViroleDT94(void) const;
    void setGeneratricHeightViroleDT94(bool p_value);

    quint32 getGeneratrixPerimeterNb(void) const;
    quint32 getGeneratrixPerimeterSpacingCm(void) const;
    quint32 getGeneratrixHeightNbMeasure(void) const;
    quint32 getGeneratrixHeightSpacingCmComputed(void) const;

    quint32 getCircularHeightMode(void) const;
    void setCircularHeightMode(quint32 p_value);
    quint32 getCircularHeightNbFree(void) const;
    void setCircularHeightNbFree(quint32 p_value);
    quint32 getCircularHeightNbVirole(void) const;
    void setCircularHeightNbVirole(quint32 p_value);
    quint32 getCircularHeightNbMeasureByVirole(void) const;
    void setCircularHeightNbMeasureByVirole(quint32 p_value);
    bool getCircularHeightViroleDT94(void) const;
    void setCircularHeightViroleDT94(bool p_value);
    quint32 getCircularPerimeterMode(void) const;
    void setCircularPerimeterMode(quint32 p_value);
    quint32 getCircularPerimeterNbFree(void) const;
    void setCircularPerimeterNbFree(quint32 p_value);
    quint32 getCircularPerimeterSpacingCm(void) const;
    void setCircularPerimeterSpacingCm(quint32 p_value);

    quint32 getCircularHeightNb(void) const;
    quint32 getCircularHeightSpacingCm(void) const;
    quint32 getCircularPerimeterNbMeasure(void) const;
    quint32 getCircularPerimeterSpacingCmComputed(void) const;

    // -- XmlProject
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    PTankShell & operator= (const PTankShell & p_value);
    bool operator== (const PTankShell & p_value);
    Piece *getCopy() const override;

protected :

    // -- Behaviour
    void init();
    STankFeature * m_pSTankFeature;  ///< Parent data

    // -- Parameters (generals / common)
    void deleteViroles();

    QList<Virole *> m_lVirole;
    quint32 m_operatorMode;                 ///< value in TankShellOperatingMode

    // -- Generatrix Operating Mode
    // Parameters
    quint32 m_genGeneratixPerimeterMode;    ///< value in GeneratrixPerimeterMode
    quint32 m_genGeneratixPerimeterNbFree;
    quint32 m_genGeneratrixHeightMode;      ///< value in GeneratrixHeightMode
    quint32 m_genGeneratrixHeightNbFree;
    quint32 m_genGeneratrixHeightSpacingCm;
    quint32 m_genGeneratrixHeightNbVirole;
    quint32 m_genGeneratrixHeightNbMeasureByVirole;
    bool m_genGeneratrixHeightViroleDT94;   ///< if false, use m_genGeneratrixHeightNbMeasureByVirole

    // Properties
    quint32 m_genGeneratrixPerimeterNb;
    quint32 m_genGeneratrixPerimeterSpacingCm;
    quint32 m_genGeneratrixHeightNbMeasure;
    quint32 m_genGeneratrixHeightSpacingCmComputed;

    // -- Circular Operating Mode
    // Parameters
    quint32 m_genCircularHeightMode;        ///< value in GeneratrixHeightMode
    quint32 m_genCircularHeightNbFree;
    quint32 m_genCircularHeightNbVirole;
    quint32 m_genCircularHeightNbMeasureByVirole;
    bool m_genCircularHeightViroleDT94;   ///< if false, use m_genCircularHeightNbMeasureByVirole
    quint32 m_genCircularPerimeterMode;     ///< value in CircularPerimeterMode
    quint32 m_genCircularPerimeterNbFree;
    quint32 m_genCircularPerimeterSpacingCm;

    // Properties
    quint32 m_genCircularHeightNb;
    quint32 m_genCircularHeightSpacingCm;
    quint32 m_genCircularPerimeterNbMeasure;
    quint32 m_genCircularPerimeterSpacingCmComputed;

};


