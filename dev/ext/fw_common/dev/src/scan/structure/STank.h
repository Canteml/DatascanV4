/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : STank.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/structure/Structure.h>
class PTankRoof;
class PTankShell;

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class STankFeature

/**
 * @brief the STankFeature class
 *
 */
class STankFeature : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    STankFeature();
    STankFeature(const STankFeature & p_value);
    ~STankFeature();

    // -- Parameters

    QString getTankSerialNumber() const;
    void setTankSerialNumber(const QString &);

    qreal getTankHeight(void) const;
    void setTankHeight(qreal p_value);

    qreal getTankDiameterCm(void) const;
    qreal getTankPerimeterCm(void) const;
    void setTankDiameterCm(qreal p_value);
    void setTankPerimeterCm(qreal p_value);        

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    STankFeature & operator= (const STankFeature & p_value);
    bool operator== (const STankFeature & p_value);
    virtual STankFeature * getCopy() const;

signals :
    void evTankSerialNumberChanged();

protected :

    // -- Behaviour
    void init();

    // -- Parameters
    QString m_tankSerialNumber;
    qreal m_tankHeightCm;
    qreal m_tankDiameterCm;
};



// ----------------------------------------------------------------------------
// -- Class STank

/**
 * @brief the STank class
 *
 */
class STank : public Structure
{
    Q_OBJECT
public :

    explicit STank();
    explicit STank(const STank & p_value);
    ~STank();

    // -- Parameters
    STankFeature * getTankFeature() const;
    void setTankFeature(const STankFeature & p_value);

    PTankRoof * getRoof(void) const;
    void setRoof(PTankRoof * p_roof = nullptr);     ///< if null, a new roof is created
    void removeRoof();

    PTankShell * getShell(void) const;
    void setShell(PTankShell * p_shell = nullptr);     ///< if null, a new Shell is created
    void removeShell();

    // -- Properties
    virtual QString getStructName(void) override;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    STank & operator= (const STank & p_value);
    bool operator== (const STank & p_value);
    virtual Structure * getCopy() const override;

protected :

    // -- Composition
    void init();
    virtual void deletePieces() override;    ///< derived classes must dereference their pieces

    // - Parameters

    STankFeature * m_pSTankFeature;
    PTankRoof * m_pPTankRoof;
    PTankShell * m_pPTankShell;
};

