/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Structure.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/Structure.h>
#include <scan/structure/PTankRoof.h>
#include <scan/structure/PTankShell.h>

// -- Qt includes


// ----------------------------------------------------------------------------
// -- Class StructureLocalisation

/**
 * @brief the StructureLocalisation class
 *
 */


StructureLocalisation::StructureLocalisation() : XmlProjectLoggable("StructureLocalisation")
{
    m_localisationPlace = "";
    m_localisationSector = "";
    m_localisationReference = "";
    m_localisationSerialNumber = "";
}

StructureLocalisation::~StructureLocalisation()
{

}

// -- Parameters
QString StructureLocalisation::getLocalisationPlace(void) const
{
    return m_localisationPlace;
}

void StructureLocalisation::setLocalisationPlace(const QString & p_value)
{
    m_localisationPlace = p_value;
}

QString StructureLocalisation::getLocalisationSector(void) const
{
    return m_localisationSector;
}

void StructureLocalisation::setLocalisationSector(const QString & p_value)
{
    m_localisationSector = p_value;
}

QString StructureLocalisation::getLocalisationReference(void) const
{
    return m_localisationReference;
}

void StructureLocalisation::setLocalisationReference(const QString & p_value)
{
    m_localisationReference = p_value;
}

QString StructureLocalisation::getLocalisationSerialNumber() const
{
    return m_localisationSerialNumber;
}

void StructureLocalisation::setLocalisationSerialNumber(const QString &p_value)
{
    m_localisationSerialNumber = p_value;
}


// -- XmlProject
bool StructureLocalisation::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_localisationPlace);
    bRet &= XML_READ_UNIQUE(QString, m_localisationSector);
    bRet &= XML_READ_UNIQUE(QString, m_localisationReference);
    bRet &= XML_READ_UNIQUE(QString, m_localisationSerialNumber);

    return bRet;
}

bool StructureLocalisation::customLoadDefaultParam()
{
    bool bRet = true;
    m_localisationPlace = "";
    m_localisationSector = "";
    m_localisationReference = "";
    m_localisationSerialNumber = "";
    return bRet;
}

bool StructureLocalisation::customApplyParam()
{
    return true;
}

bool StructureLocalisation::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_localisationPlace);
    bRet &= XML_WRITE_UNIQUE(m_localisationSector);
    bRet &= XML_WRITE_UNIQUE(m_localisationReference);
    bRet &= XML_WRITE_UNIQUE(m_localisationSerialNumber);
    return bRet;
}


// -- operator
StructureLocalisation & StructureLocalisation::operator= (const StructureLocalisation & p_value)
{
    if(this != &p_value) {
        m_localisationPlace = p_value.getLocalisationPlace();
        m_localisationSector = p_value.getLocalisationSector();
        m_localisationReference = p_value.getLocalisationReference();
        m_localisationSerialNumber = p_value.getLocalisationSerialNumber();
        emit evLocalisationChanged();
    }
    return (*this);
}

bool StructureLocalisation::operator== (const StructureLocalisation & p_value)
{
    return (m_localisationPlace == p_value.getLocalisationPlace()) &&
            (m_localisationSector == p_value.getLocalisationSector()) &&
            (m_localisationReference == p_value.getLocalisationReference() &&
            (m_localisationSerialNumber == p_value.getLocalisationSerialNumber()));
}


// ----------------------------------------------------------------------------
// -- Class Structure


QString Structure::getStructureTypeName(quint32 e)
{
    const std::map<quint32,const char*> MyEnumStrings {
           { Structure::StructureType::STR_UNDEF, "UNDEFINED" },
           { Structure::StructureType::STR_TANK, "TANK" },
           { Structure::StructureType::STR_FREE, "FREE" }
    };
    auto   it  = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? QString("Out of range") : QString(it->second);
}

// -- Methods

Structure::Structure(quint32 p_type) : XmlProjectLoggable("Structure"),
    m_structureType(p_type)
{
    m_pLocalisation = new StructureLocalisation();
    connect(m_pLocalisation, &StructureLocalisation::evLocalisationChanged, this, &Structure::requestUpdateStructureName);
}

Structure::Structure(const Structure &p_structure) : QObject(), XmlProjectLoggable("Structure"),
    m_structureType(p_structure.getStructureType())
{
    m_pLocalisation = new StructureLocalisation();
    (*this) = p_structure;
}

Structure::~Structure()
{
    deletePieces();
    delete m_pLocalisation;
    m_pLocalisation = nullptr;
}

QString Structure::getStructureTypeString()
{
    QString sRet;
    if( STR_UNDEF == m_structureType ) {
        sRet = tr("Non définit");
    } else if( STR_TANK == m_structureType ) {
        sRet = tr("Bac");
    } else if( STR_FREE == m_structureType ) {
        sRet = tr("Libre");
    } else {
        fLogError("No name for the type : " + QString::number(m_structureType));
    }
    return sRet;
}


// -- Behaviour
const QList<Piece*> * Structure::getListPieces(void) const
{
    return &m_lPieces;
}

void Structure::setLocalisation(const StructureLocalisation &p_value)
{
    (*m_pLocalisation) = p_value;
}

const StructureLocalisation * Structure::getLocalisation(void) const
{
    return m_pLocalisation;
}

void Structure::useDefaultStructure()
{
    deletePieces();
    m_pLocalisation->setLocalisationPlace("");
    m_pLocalisation->setLocalisationSector("");
    m_pLocalisation->setLocalisationReference("");
    m_pLocalisation->setLocalisationSerialNumber("");
}

// -- Parameters
quint32 Structure::getStructureType() const // value in StructureType
{
    return m_structureType;
}   

// -- XmlProject
bool Structure::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;

    //bRet &= XML_READ_UNIQUE(quint32, m_structureType); <= set into constructor
    bRet &= m_pLocalisation->loadParam(p_pParent);

    // -- load list pieces
    deletePieces();
    // find element list
    QDomElement eList = p_pParent->firstChildElement("listPiece");
    if( !eList.isNull() ) {
        QDomElement ePiece = eList.firstChildElement("Piece");
        while( !ePiece.isNull() && bRet ) {
            // load zoning
            Piece * p = Piece::loadPiece(&ePiece);
            if( nullptr != p ) {
                m_lPieces.append(p);
            } else {
                fLogError(QString("Fail to load Piece : '%1'").arg(ePiece.text()));
                bRet = false;
            }
            // next iteration
            ePiece = ePiece.nextSiblingElement("Piece").toElement();
        }
    } else {
        fLogError("missing tag 'listPiece'");
        bRet = false;
    }

    return bRet;
}

bool Structure::customLoadDefaultParam()
{
    bool bRet = m_pLocalisation->customLoadDefaultParam();
    deletePieces();
    return bRet;
}

bool Structure::customApplyParam()
{
    bool bRet = m_pLocalisation->customApplyParam();
    return bRet;
}

bool Structure::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_structureType);
    bRet &= m_pLocalisation->saveParam(p_pDoc, p_pParent);

    // save list pieces
    QDomElement eList = p_pParent->firstChildElement("listPiece");
    if( eList.isNull() ) {
        // Parameter doesn't exist, make it !
        eList = p_pDoc->createElement("listPiece");
        p_pParent->appendChild(eList);
    } else {
        // Parameter found, change its value with a new element
        QDomNode eItem = eList.firstChild();
        while(!eItem.isNull()) {
            eList.removeChild(eItem);
            eItem = eItem.nextSibling();
        }
    }

    for(auto i : qAsConst(m_lPieces)) {
        QDomElement eZone = p_pDoc->createElement(i->getXmlSectionName());
        i->customSaveParam(p_pDoc, &eZone);
        eList.appendChild(eZone);
    }

    return bRet;
}

QString Structure::getStructName()
{
    return QString("%1 : %2 - %3").arg(getStructureTypeString())
            .arg(getLocalisation()->getLocalisationPlace())
            .arg(getLocalisation()->getLocalisationReference());
}


// -- operator
Structure & Structure::operator= (const Structure & p_value)
{
    if( this != &p_value ) {
        if( m_structureType != p_value.getStructureType() ) {
            fLogWarning(QString("Changing structure type (%1 => %2), can be inconsistent").arg(p_value.getStructureType()).arg(p_value.getStructureType()));
            m_structureType = p_value.getStructureType();
        }
        // copy localisation
        (*m_pLocalisation) = (*p_value.getLocalisation());
        // -- remove current pieces
        deletePieces();
        // -- copy pieces
        const QList<Piece*> * lP = p_value.getListPieces();
        for(auto i : (*lP) ) {
            Piece * p = i->getCopy();
            m_lPieces.append(p);
        }
    }
    return (*this);
}

bool Structure::operator== (const Structure & p_value)
{
    bool bRet = true;
    bRet &= (m_structureType == p_value.getStructureType());
    bRet &= (*m_pLocalisation) == (*p_value.getLocalisation());
    const QList<Piece*> * lP = p_value.getListPieces();
    if( lP->size() != m_lPieces.size() ) {
        bRet = false;
    } else {
        int i = 0;
        for(auto p : (*lP) ) {
            if( p != m_lPieces.at(i)) {
                bRet = false;
                break;
            }
            i++;
        }
    }
    return bRet;
}

Structure *Structure::getCopy() const
{
    return new Structure(*this);
}

void Structure::requestUpdateStructureName()
{
    emit structNameChanged();
}



// -- Composition
void Structure::deletePieces()
{
    for(auto i : qAsConst(m_lPieces)) {
        delete i;
    }
    m_lPieces.clear();
}
