/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Piece.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
class Zoning;

// -- Qt includes


/**
 * @brief the Piece class
 *
 */
class Piece : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :

    enum PieceType {
        PIE_UNDEF,
        PIE_TANK_SHELL,
        PIE_TANK_ROOF,
        PIE_FREE        ///< declared in SFree.h
    };
    static QString getPieceTypeName(quint32 e);

    // -- Methods

    explicit Piece(quint32 p_type);
    explicit Piece(const Piece& p_value);
    virtual ~Piece();

    quint32 getPieceType(void) const;      ///< value in Piece::PieceType


    // -- Behaviour
    virtual bool generateZoning(quint32 p_ZoningType);     ///< use data of the derived piece to build m_lZoning. Call parent method first

    // -- Parameters
    QString getPieceOriginLocation(void) const;
    void setPieceOriginLocation(const QString& p_value);
    Zoning* getZoning(qint32 p_index) const;
    const QList<Zoning *> &getZoningList() const;


    // -- XmlProject
    static Piece *loadPiece(QDomElement * p_pParent);   ///< p_pParent must be node "Piece"

    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    Piece & operator= (const Piece & p_value);
    bool operator== (const Piece & p_value);
    virtual Piece *getCopy() const;

protected :

    // -- Behaviour
    virtual void deleteZoning();

    quint32 m_pieceType;

    // -- Parameters
    QString m_PieceOriginLocation;  ///< where is placed the system coordinates on the piece
    QList<Zoning* > m_lZoning;
};

