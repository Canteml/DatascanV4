/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Structure.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class Piece;

// -- Qt includes
#include <QObject>


// ----------------------------------------------------------------------------
// -- Class StructureLocalisation

/**
 * @brief the StructureLocalisation class
 *
 * FOR THANK (in first time)
 *
 */
class StructureLocalisation : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    explicit StructureLocalisation();
    ~StructureLocalisation();

    // -- Parameters
    QString getLocalisationPlace(void) const;
    void setLocalisationPlace(const QString & p_value);
    QString getLocalisationSector(void) const;
    void setLocalisationSector(const QString & p_value);
    QString getLocalisationReference(void) const;
    void setLocalisationReference(const QString & p_value);    
    QString getLocalisationSerialNumber(void) const;
    void setLocalisationSerialNumber(const QString & p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    StructureLocalisation & operator= (const StructureLocalisation & p_value);
    bool operator== (const StructureLocalisation & p_value);

signals :
    void evLocalisationChanged();

protected :

    // -- Parameters
    QString m_localisationPlace;    ///< customer place where examen is done
    QString m_localisationSector;
    QString m_localisationReference;
    QString m_localisationSerialNumber;
};

// ----------------------------------------------------------------------------
// -- Class Structure

/**
 * @brief the Structure class
 *
 * Eache derived class must instanciate its own pieces and manage the list of pieces (constructor)
 * The Structure class will load/save pieces automatically, just get references on derived classes.
 *
 */
class Structure : public QObject, public XmlProjectLoggable
{
    Q_OBJECT

    // -- Structure
    Q_PROPERTY(QString structName READ getStructName NOTIFY structNameChanged)

public :

    // -- Type
    enum StructureType {
        STR_UNDEF,
        STR_TANK,
        STR_FREE
    };

    static QString getStructureTypeName(quint32 e);     ///< return a string with the name of the Structure type

    // -- Methods

    explicit Structure(quint32 p_type);
    explicit Structure(const Structure& p_structure);
    virtual ~Structure();

    QString getStructureTypeString();   ///< return a string with the TRANSLATED name of the Structure type

    // -- Behaviour
    const QList<Piece *> *getListPieces(void) const;
    void setLocalisation(const StructureLocalisation & p_value);
    const StructureLocalisation * getLocalisation(void) const;

    virtual void useDefaultStructure(void);

    // -- Parameters
    quint32 getStructureType() const;   ///< value in StructureType
    QString getStructureTypeName() const;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Properties
    virtual QString getStructName(void);

    // -- operator
    Structure & operator= (const Structure & p_value);
    bool operator== (const Structure & p_value);
    virtual Structure * getCopy() const;

signals :
    void structNameChanged();

protected slots :
    void requestUpdateStructureName();  ///< call this slot when properties of the structure have changed and the name must be updated

protected :

    // -- Composition
    virtual void deletePieces();    ///< derived classes must dereference their pieces
    QList<Piece*> m_lPieces;
    StructureLocalisation * m_pLocalisation;

    // -- Parameters
    quint32 m_structureType;
};

