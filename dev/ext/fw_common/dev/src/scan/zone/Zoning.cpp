/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Zoning.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- internal includes
#include <scan/zone/Zoning.h>
#include <scan/zone/Zone.h>
#include <scan/zone/Location.h>

// -- Qt includes


// ----------------------------------------------------------------------------
// -- Class Zoning


Zoning::Zoning(quint32 p_type) : XmlProjectLoggable("Zoning")
{
    m_zoningType = p_type;
    m_zoningId = 0;
    m_zoningOperatingPath = OPERATING_MANUAL;
}

Zoning::Zoning(const Zoning &p_zoning) : QObject(), XmlProjectLoggable("Zoning")
{
    (*this) = p_zoning;
}

Zoning::Zoning(quint32 p_type, const QString &p_zoningName) : XmlProjectLoggable("Zoning")
{
    m_zoningType = p_type;
    m_zoningName = p_zoningName;
    m_zoningId = 0;
    m_zoningOperatingPath = OPERATING_MANUAL;
}

Zoning::~Zoning()
{
    removeAllZone();
    removeAllLocation();
}



// -- Behaviour
quint32 Zoning::getZoningId(void) const
{
    return m_zoningId;
}

void Zoning::setZoningId(quint32 p_value)
{
    m_zoningId = p_value;
}

quint32 Zoning::getZoningType() const
{
    return m_zoningType;
}

void Zoning::setZoningType(quint32 p_value)
{
    m_zoningType = p_value;
}

// -- Data
void Zoning::setZoneMatrixSize(quint32 p_x, quint32 p_y)
{
    removeAllZone();

    // set X :
    m_matZone.resize(p_x);
    for(quint32 iX = 0; iX<p_x; iX++) {
        QVector<Zone *> * x =  &(m_matZone[iX]);
        x->resize(p_y);
        for(quint32 iY = 0; iY<p_y; iY++) {
            Zone * z = new Zone();
            (*x)[iY] = z;
        }
    }
}

qint32 Zoning::getZoneMatrixSizeX() const
{
    return m_matZone.size();
}

qint32 Zoning::getZoneMatrixSizeY() const
{
    qint32 sy = 0;
    if( 0 < m_matZone.size() ) {
        sy = m_matZone.at(0).size();
    }
    return sy;
}

Zone * Zoning::getZoneItem(int p_x, int p_y) const
{
    Zone * pRet = nullptr;
    if( p_x < m_matZone.size() ) {
        if( p_y < m_matZone.at(0).size() ) {
            pRet = m_matZone.at(p_x).at(p_y);
        }
    }
    return pRet;
}

void Zoning::removeAllZone()
{
    // delete zone
    int iMatX = getZoneMatrixSizeX();

    for(int x = 0 ; x < iMatX; x++) {
        QVector<Zone *> * pX = &m_matZone[x];
        while( pX->size() ) {
            delete pX->takeFirst();
        }
    }
    m_matZone.clear();
}

void Zoning::addRow(int p_index)
{
    // add one item for each X list :
    for(qint32 iX = 0; iX<m_matZone.size(); iX++) {
        QVector<Zone *> * x =  &(m_matZone[iX]);
        x->insert(p_index, new Zone());
    }
}

void Zoning::addColumn(int p_index)
{
    // add one list on X
    QVector<Zone *> x;
    int sY = getZoneMatrixSizeY();
    x.resize(sY);
    for(qint32 iY = 0; iY<sY; iY++) {
        Zone * z = new Zone();
        x[iY] = z;
    }
    m_matZone.insert(p_index, x);
}

void Zoning::removeRow(int p_index)
{
    // remove one item for each X list :
    for(qint32 iX = 0; iX<m_matZone.size(); iX++) {
        delete m_matZone[iX][p_index];
        m_matZone[iX].removeAt(p_index);
    }
}

void Zoning::removeColumn(int p_index)
{
    // remove one list on X
    QVector<Zone *> * x = &(m_matZone[p_index]);
    for(int iY = 0; iY<x->size(); iY++) {
        delete x->at(iY);
    }
    m_matZone.removeAt(p_index);
}

quint32 Zoning::getLocationListSize() const
{
    return m_lLocation.size();
}

Location * Zoning::getLocation(int p_index) const
{
    Location * pRet = nullptr;
    if( p_index < m_lLocation.size() && 0 <= p_index ) {
        pRet = m_lLocation.at(p_index);
    } else {
        fLogError(QString("Bad index : %1 (size = %2)").arg(p_index).arg(m_lLocation.size()));
    }
    return pRet;
}

void Zoning::addLocation(int p_index, Location * p_pLocation)
{
    if( p_index == m_lLocation.size() ) {
        m_lLocation.append(p_pLocation);
    } else if( p_index < m_lLocation.size() && 0 < p_index) {
        m_lLocation.insert(p_index, p_pLocation);
    } else {
        fLogError(QString("Bad index : %1 (size = %2)").arg(p_index).arg(m_lLocation.size()));
    }
}

void Zoning::removeLocation(int p_index)
{
    if( p_index < m_lLocation.size() && 0 < p_index) {
        Location * pLoc = m_lLocation.at(p_index);
        m_lLocation.removeAt(p_index);
        delete pLoc;
    } else {
        fLogError(QString("Bad index : %1 (size = %2)").arg(p_index).arg(m_lLocation.size()));
    }
}

void Zoning::removeAllLocation()
{
    // delete m_lLocation
    for(auto i : qAsConst(m_lLocation)) {
        delete i;
    }
    m_lLocation.clear();
}

// -- Convenient methods
bool Zoning::isOperatingPathCW() const
{
    bool bRet = (m_zoningOperatingPath == OPERATING_VERTICAL_CW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_DU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_UD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_DU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_UD);
    return bRet;
}

bool Zoning::isOperatingPathCCW() const
{
    bool bRet = (m_zoningOperatingPath == OPERATING_VERTICAL_CCW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_DU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_UD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_DU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_UD);
    return bRet;
}

bool Zoning::isOperatingVertical() const
{
    bool bRet = (m_zoningOperatingPath == OPERATING_VERTICAL_CW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_DU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CW_UD ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_DU ||
                 m_zoningOperatingPath == OPERATING_VERTICAL_CCW_UD );
    return bRet;
}


bool Zoning::isOperatingHorizontal() const
{
    bool bRet = (m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_DU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CW_UD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_ZZDU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_ZZUD ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_DU ||
                 m_zoningOperatingPath == OPERATING_HORIZONTAL_CCW_UD );
    return bRet;
}

bool Zoning::isOperatingManual() const
{
    bool bRet = m_zoningOperatingPath == OPERATING_MANUAL;

    return bRet;
}

// -- Parameters
QString Zoning::getZoningName(void) const
{
    return m_zoningName;
}

void Zoning::setZoningName(const QString & p_value)
{
    m_zoningName = p_value;
}

quint32 Zoning::getZoningOperatingPath() const
{
    return m_zoningOperatingPath;
}

void Zoning::setZoningOperatingPath(quint32 p_value)
{
    m_zoningOperatingPath = p_value;
    emit evZoningOperatingPathChanged();
}

// -- XmlProject
bool Zoning::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(quint32, m_zoningId);
    bRet &= XML_READ_UNIQUE(quint32, m_zoningType);
    bRet &= XML_READ_UNIQUE(QString, m_zoningName);
    bRet &= XML_READ_UNIQUE(quint32, m_zoningOperatingPath);

    // -- read matrix
    removeAllZone();
    // find matrices
    QDomElement eMat = p_pParent->firstChildElement("matZone");
    if( !eMat.isNull() ) {
        bool bOk;
        quint32 iMatX = eMat.attributeNode("x").value().toUInt(&bOk);
        if( !bOk ) {
            fLogError("fail to read attributs 'x' of matZone");
            bRet = false;
        } else {
            quint32 iMatY = eMat.attributeNode("y").value().toUInt(&bOk);
            if( !bOk ) {
                fLogError("fail to read attributs 'y' of matZone");
                bRet = false;
            } else {
                // Create struct and load it
                QDomElement eZone = eMat.firstChildElement("Zone");
                bool bDefaultZone = false;
                if( !eZone.isNull() ) {
                    m_matZone.resize(iMatX);
                    for(quint32 x = 0 ; x < iMatX; x++) {
                        QVector<Zone *> * pX = &m_matZone[x];
                        pX->resize(iMatY);
                        for(quint32 y = 0 ; y < iMatY; y++) {
                            Zone * pY = new Zone();
                            (*pX)[y] = pY;  // affect
                            if( eZone.isNull() ) {
                                //fLogWarning(QString("Fail to found zone for X=%1(/%2) Y=%3(/%4)").arg(x).arg(iMatX).arg(y).arg(iMatY));
                                //break;
                                bDefaultZone = true;
                            } else {
                                pY->customLoadParam(&eZone);
                            }
                            eZone = eZone.nextSiblingElement("Zone");
                        }
                        if( !bRet /*|| eZone.isNull()*/ ) {
                            break;  // exit if an error occured
                        }
                    }

                    if( bDefaultZone ) {
                        fLogWarning("default zone loaded");
                    }
                }
            }
        }
    } else {
        fLogError("Fail to find 'matZone'");
        bRet = false;
    }

    // read location
    removeAllLocation();
    QDomElement eListLoc = p_pParent->firstChildElement("listLoc");
    if( !eListLoc.isNull() ) {
        QDomElement eLoc = eListLoc.firstChildElement("Location");
        while(!eLoc.isNull() && bRet) {
            Location * pLoc = new Location();
            bRet &= pLoc->customLoadParam(&eLoc);
            eLoc = eLoc.nextSiblingElement("Location");
        }
    } else {
        fLogError("Fail to find 'listLoc'");
        bRet = false;
    }

    return bRet;
}

bool Zoning::customLoadDefaultParam()
{
    bool bRet = true;
    m_zoningId = 0;
    m_zoningType = TYPE_UNDEF;
    m_zoningName = "";
    m_zoningOperatingPath = OPERATING_MANUAL;
    removeAllZone();
    removeAllLocation();
    return bRet;
}

bool Zoning::customApplyParam()
{
    bool bRet = true;
    return bRet;
}

bool Zoning::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_zoningType);
    bRet &= XML_WRITE_UNIQUE(m_zoningId);
    bRet &= XML_WRITE_UNIQUE(m_zoningName);
    bRet &= XML_WRITE_UNIQUE(m_zoningOperatingPath);

    // -- write matrix
    QDomElement eMat = p_pParent->firstChildElement("matZone");
    if( eMat.isNull() ) {
        // Parameter doesn't exist, make it !
        eMat = p_pDoc->createElement("matZone");
        p_pParent->appendChild(eMat);
    } else {
        // Parameter found, change its value with a new element
        QDomElement newE = p_pDoc->createElement("matZone");
        p_pParent->replaceChild(eMat, newE);
        eMat = newE;
    }

    // set attributs
    QDomAttr aX = p_pDoc->createAttribute("x");
    aX.setValue(QString::number(getZoneMatrixSizeX()));
    QDomAttr aY = p_pDoc->createAttribute("y");
    aY.setValue(QString::number(getZoneMatrixSizeY()));
    eMat.setAttributeNode(aX);
    eMat.setAttributeNode(aY);

    // add elements
    for(auto x : qAsConst(m_matZone)) {
        for(auto y : qAsConst(x)) {
//            QDomElement eZone = p_pDoc->createElement("zone");
//            bRet &= y->customSaveParam(p_pDoc, &eZone);
//            eMat.appendChild(eZone);
            bRet &= y->saveParam(p_pDoc, &eMat);

            if( !bRet ) {
                fLogError("Fail to write element 'zone'");
                break;  // exit if an error occured
            }
        }
        if( !bRet ) {
            break;  // exit if an error occured
        }
    }

    // -- write list
    QDomElement eListLoc = p_pParent->firstChildElement("listLoc");
    if( eListLoc.isNull() ) {
        // Parameter doesn't exist, make it !
        eListLoc = p_pDoc->createElement("listLoc");
        p_pParent->appendChild(eListLoc);
    } else {
        QDomElement newE = p_pDoc->createElement("listLoc");
        p_pParent->replaceChild(eListLoc, newE);
        eListLoc = newE;
    }
    // add elements of the list
    for(auto i : qAsConst(m_lLocation)) {
        bRet &= i->saveParam(p_pDoc, &eListLoc);
        if( !bRet ) {
            fLogError("Fail to write element 'Location'");
            break;  // exit if an error occured
        }
    }

    return bRet;
}

// -- Operators

bool Zoning::operator==(const Zoning &p_value) const
{
    bool bRet = true;
    // -- Behaviour
    if( p_value.getZoningType() != m_zoningType || p_value.getZoningName() != m_zoningName || p_value.getZoningOperatingPath() != m_zoningOperatingPath ) {
        // note : ID is not checked
       bRet = false;
    } else {
        // -- data
        // matrix
        if( p_value.getZoneMatrixSizeX() != getZoneMatrixSizeX() || p_value.getZoneMatrixSizeY() != getZoneMatrixSizeY() ) {
            bRet = false;
        } else {
            int iX = 0, iY = 0;
            for(int i=0; i<m_matZone.size(); i++) {
                const QVector<Zone *> * x = &(m_matZone.at(i));
                for(int j=0; j<x->size(); j++) {
                    const Zone * y = x->at(i);
                    bRet &= y == p_value.getZoneItem(iX, iY);
                    if( !bRet ) {
                        break;
                    }
                    iY++;
                }
                if( !bRet ) {
                    break;
                }
                iX++;
            }

            if( bRet ) {
                // list
                if( p_value.getLocationListSize() != getLocationListSize() ) {
                    bRet = false;
                } else {
                    int iL = 0;
                    for(auto i : qAsConst(m_lLocation)) {
                        bRet &= i == p_value.getLocation(iL);
                        if( !bRet ) {
                            break;
                        }
                        iL++;
                    }
                }
            }
        }
    }
    return bRet;
}

Zoning &Zoning::operator=(const Zoning &p_value)
{
    if( this != &p_value ) {
        // -- behaviour
        setZoningName(p_value.getZoningName());
        setZoningType(p_value.getZoningType());
        setZoningOperatingPath(p_value.getZoningOperatingPath());
        // zoning ID : \todo

        // -- data
        // matrix
        removeAllZone();
        //setZoneMatrixSize(p_value.getZoneMatrixSizeX(), p_value.getZoneMatrixSizeY());
        // allocate matrix :
        // set X :
        int sX = p_value.getZoneMatrixSizeX();
        int sY = p_value.getZoneMatrixSizeY();

        m_matZone.resize(sX);
        for(int iX = 0; iX<sX; iX++) {
            QVector<Zone *> * x =  &(m_matZone[iX]);
            x->resize(sY);
        }
        // copy items
        for(qint32 iX = 0; iX < sX; iX++) {
            for(qint32 iY = 0; iY < sY; iY++ ) {
                Zone *pZone = p_value.getZoneItem(iX, iY)->copy();
                m_matZone[iX][iY] = pZone;
            }
        }

        // list
        removeAllLocation();
        for( quint32 iL = 0; iL < p_value.getLocationListSize(); iL++ ) {
            Location * pLoc = new Location(*p_value.getLocation(iL));
            m_lLocation.append(pLoc);
        }
    }

    return (*this);
}

Zoning *Zoning::getCopy() const
{
    return new Zoning((*this));
}
