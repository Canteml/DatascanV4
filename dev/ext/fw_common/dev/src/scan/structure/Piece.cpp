/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Piece.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/Piece.h>
#include <scan/zone/Zoning.h>
#include <scan/structure/PTankShell.h>
#include <scan/structure/PTankRoof.h>
#include <scan/structure/PFree.h>
#include <scan/structure/SFree.h>   // contains also PieceFree

// -- Qt includes

enum PieceType {
    PIE_UNDEF,
    PIE_TANK_SHELL,
    PIE_TANK_ROOF
};

Piece::Piece(quint32 p_type) : XmlProjectLoggable("Piece"), m_pieceType(p_type)
{

}

Piece::Piece(const Piece& p_value) : QObject(), XmlProjectLoggable("Piece"), m_pieceType(p_value.getPieceType())
{
    (*this) = p_value;
}

Piece::~Piece()
{

}

quint32 Piece::getPieceType() const
{
    return m_pieceType;
}

QString Piece::getPieceTypeName(quint32 e)
{
    const std::map<quint32,const char*> MyEnumStrings {
           { Piece::PieceType::PIE_UNDEF, "UNDEFINED" },
           { Piece::PieceType::PIE_TANK_SHELL, "TANK SHELL" },
           { Piece::PieceType::PIE_TANK_ROOF, "TANK ROOF" },
           { Piece::PieceType::PIE_FREE, "TANK FREE" }
    };
    auto   it  = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? QString("Out of range") : QString(it->second);
}



// -- Behaviour
bool Piece::generateZoning(quint32)
{
    // prepare
    deleteZoning();
    return true;
}

// -- Parameters
QString Piece::getPieceOriginLocation(void) const
{
    return m_PieceOriginLocation;
}

void Piece::setPieceOriginLocation(const QString& p_value)
{
    m_PieceOriginLocation = p_value;
}

Zoning *Piece::getZoning(qint32 p_index) const
{
    Zoning* pRet = nullptr;
    if( p_index < m_lZoning.size() ) {
        pRet = m_lZoning.at(p_index);
    } else {
        fLogError(QString("index '%1' out of list size (%2)").arg(p_index).arg(m_lZoning.size()));
    }
    return pRet;
}

const QList<Zoning *>& Piece::getZoningList() const
{
    return m_lZoning;
}

Piece *Piece::loadPiece(QDomElement *p_pSection)
{
    Piece* pRet = nullptr;
    QDomElement eType = p_pSection->firstChildElement("m_pieceType");
    if( eType.isNull() ) {
        Logger::logError("Piece *Piece::loadPiece - m_pieceType not found");
    } else {
        bool bOk;
        int t = eType.text().toInt(&bOk);
        if( !bOk ) {
            Logger::logError("Piece *Piece::loadPiece - 'Type' is not an integer : "+eType.text());
        } else {
            if( Piece::PieceType::PIE_TANK_SHELL == t ) {
                PTankShell * p = new PTankShell();
                p->loadParamSection(p_pSection);
                pRet = p;
            } else if( Piece::PieceType::PIE_TANK_ROOF == t ) {
                PTankRoof * p = new PTankRoof();
                p->loadParamSection(p_pSection);
                pRet = p;
            } else if( Piece::PieceType::PIE_FREE == t ) {
                PFree * p = new PFree();
                p->loadParamSection(p_pSection);
                pRet = p;
            } else {
                Logger::logError("Piece *Piece::loadPiece - 'Type' is not managed : "+QString::number(t));
            }
        }
    }
    return pRet;
}


// -- XmlProject
bool Piece::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    //bRet &= XML_READ_UNIQUE(quint32, m_pieceType); <= set into constructor
    bRet &= XML_READ_UNIQUE(QString, m_PieceOriginLocation);

    // list of zonings
    deleteZoning();
    // find element list
    QDomElement eList = p_pParent->firstChildElement("listZoning");
    if( !eList.isNull() ) {
        QDomElement eZoning = eList.firstChildElement("Zoning");
        while( !eZoning.isNull() && bRet ) {
            // load zoning
            Zoning * zoning = new Zoning(Zoning::TYPE_UNDEF);
            m_lZoning.append(zoning);
            bRet &= zoning->loadParamSection(&eZoning);
            // next iteration
            eZoning = eZoning.nextSiblingElement("Zoning");
        }
    } else {
        fLogError("missing tag 'listZoning'");
        bRet = false;
    }

    return bRet;
}

bool Piece::customLoadDefaultParam()
{
    bool bRet = true;
    m_PieceOriginLocation = "";
    deleteZoning();
    return bRet;
}

bool Piece::customApplyParam()
{
    bool bRet = true;
    return bRet;
}

bool Piece::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_pieceType);
    bRet &= XML_WRITE_UNIQUE(m_PieceOriginLocation);

    // --  write list of zonings
    QDomElement eList = p_pParent->firstChildElement("listZoning");
    if( eList.isNull() ) {
        // Parameter doesn't exist, make it !
        eList = p_pDoc->createElement("listZoning");
        p_pParent->appendChild(eList);
    } else {
        // Parameter found, change its value with a new element
        eList.clear();
    }

    for(auto i : qAsConst(m_lZoning)) {
        QDomElement eZone = p_pDoc->createElement("Zoning");
        i->customSaveParam(p_pDoc, &eZone);
        eList.appendChild(eZone);
    }

    return bRet;
}


// -- operator
Piece & Piece::operator= (const Piece & p_value)
{
    if( p_value.getPieceType() != m_pieceType ) {
        fLogError(QString("Trying to affect 2 different types of Piece : %1 = %2").arg(getPieceTypeName(m_pieceType).arg(getPieceTypeName(p_value.getPieceType()))));
    } else {
        setPieceOriginLocation(p_value.getPieceOriginLocation());
        // list of zoning
        deleteZoning();
        for(auto i : (p_value.getZoningList()) )  {
            m_lZoning.append(new Zoning((*i)));
        }
    }
    return (*this);
}

bool Piece::operator== (const Piece & p_value)
{
    bool bRet = true;

    if( m_pieceType != p_value.getPieceType() ) {
        bRet = false;
    } else {
        if( m_PieceOriginLocation != p_value.getPieceOriginLocation() ) {
            bRet = false;
        } else {
            if( m_lZoning.size() != p_value.getZoningList().size() ) {
                bRet = false;
            } else {
                for( int i=0; i<m_lZoning.size(); i++ ) {
                    if( m_lZoning.at(i) != p_value.getZoning(i) ) {
                        bRet = false;
                        break;
                    }
                }
            }
        }
    }

    return bRet;
}

Piece *Piece::getCopy() const
{
    return new Piece(*this);
}

void Piece::deleteZoning()
{
    for(auto i : qAsConst(m_lZoning)) {
        delete i;
    }
    m_lZoning.clear();
}
