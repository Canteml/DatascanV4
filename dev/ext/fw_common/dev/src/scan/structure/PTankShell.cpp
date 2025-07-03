/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : PTankShell.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/PTankShell.h>


// ----------------------------------------------------------------------------
// -- Class Virole


Virole::Virole() : XmlProjectLoggable("Virole")
{
    m_viroleName = "";
    m_viroleOriginalThickness = 0;
    m_viroleNbYZone = 0;
}

Virole::~Virole()
{

}

// -- Parameters
QString Virole::getViroleName() const
{
    return m_viroleName;
}

void Virole::setViroleName(const QString & p_value)
{
    m_viroleName = p_value;
}

qreal Virole::getViroleOriginalThikness(void) const
{
    return m_viroleOriginalThickness;
}

void Virole::setViroleOriginalThikness(qreal p_value)
{
    m_viroleOriginalThickness = p_value;
}

quint32 Virole::getViroleNbZone() const
{
    return m_viroleNbYZone;
}

void Virole::setViroleYNbZone(quint32 p_value)
{
    m_viroleNbYZone = p_value;
}


// -- XmlProject
bool Virole::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_viroleName);
    bRet &= XML_READ_UNIQUE(qreal, m_viroleOriginalThickness);
    bRet &= XML_READ_UNIQUE(quint32, m_viroleNbYZone);
    return bRet;
}

bool Virole::customLoadDefaultParam()
{
    m_viroleName = "";
    m_viroleOriginalThickness = 0;
    m_viroleNbYZone = 0;
    return true;
}

bool Virole::customApplyParam()
{

    return true;
}

bool Virole::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_viroleName);
    bRet &= XML_WRITE_UNIQUE(m_viroleOriginalThickness);
    bRet &= XML_WRITE_UNIQUE(m_viroleNbYZone);
    return bRet;
}


// -- operator
Virole & Virole::operator= (const Virole & p_value)
{
    if( this != &p_value ) {
        m_viroleName = p_value.getViroleName();
        m_viroleOriginalThickness = p_value.getViroleOriginalThikness();
        m_viroleNbYZone = p_value.getViroleNbZone();
    }
    return (*this);
}

bool Virole::operator== (const Virole & p_value)
{
    return (m_viroleName == p_value.getViroleName()) &&
            (m_viroleOriginalThickness == p_value.getViroleOriginalThikness()) &&
            (m_viroleNbYZone == p_value.getViroleNbZone());
}

// ----------------------------------------------------------------------------
// -- Class PTankShell


PTankShell::PTankShell() : Piece(PIE_TANK_SHELL)
{
    init();
}

PTankShell::PTankShell(const PTankShell &p_value) : Piece(PIE_TANK_SHELL)
{
    init();
    *(this) = p_value;
}

void PTankShell::init()
{
    // -- Behaviour
    m_pSTankFeature = nullptr;

    // -- Parameters (generals / common)
    m_operatorMode = OPERATING_GENERATRIX;

    // -- Generatrix Operating Mode
    // Parameters
    m_genGeneratixPerimeterMode = GENERATRIX_P_DT94;    ///< value in GeneratrixPerimeterMode
    m_genGeneratixPerimeterNbFree = 0;
    m_genGeneratrixHeightMode = GENERATRIX_H_VIROLE;      ///< value in GeneratrixHeightMode
    m_genGeneratrixHeightNbFree = 0;
    m_genGeneratrixHeightSpacingCm = 50;
    m_genGeneratrixHeightNbVirole = 0;
    m_genGeneratrixHeightNbMeasureByVirole = 0;
    m_genGeneratrixHeightViroleDT94 = true;   ///< if false, use m_genGeneratrixHeightNbMeasureByVirole

    // Properties
    m_genGeneratrixPerimeterNb = 0;
    m_genGeneratrixPerimeterSpacingCm = 0;
    m_genGeneratrixHeightNbMeasure = 0;
    m_genGeneratrixHeightSpacingCmComputed = 0;

    // -- Circular Operating Mode
    // Parameters
    m_genCircularHeightMode = CIRCULAR_H_VIROLE;        ///< value in GeneratrixHeightMode
    m_genCircularHeightNbFree = 0;
    m_genCircularHeightNbVirole = 0;
    m_genCircularHeightNbMeasureByVirole = 0;
    m_genCircularHeightViroleDT94 = true;   ///< if false, use m_genCircularHeightNbMeasureByVirole
    m_genCircularPerimeterMode = CIRCULAR_P_SPACING;     ///< value in CircularPerimeterMode
    m_genCircularPerimeterNbFree = 0;
    m_genCircularPerimeterSpacingCm = 50;

    // Properties
    m_genCircularHeightNb = 0;
    m_genCircularHeightSpacingCm = 0;
    m_genCircularPerimeterNbMeasure = 0;
    m_genCircularPerimeterSpacingCmComputed = 0;
}

PTankShell::~PTankShell()
{
    deleteViroles();
}

void PTankShell::setTankFeature(STankFeature *p_pSTankFeature)
{
    m_pSTankFeature = p_pSTankFeature;
}

quint32 PTankShell::getNbOfVirole() const
{
    return m_lVirole.size();
}

Virole*  PTankShell::addVirole(qint32 p_viroleIndex)
{
    Virole* pRet = nullptr;
    if( -1 == p_viroleIndex ) {
        p_viroleIndex = m_lVirole.size();
    }

    if( p_viroleIndex <= m_lVirole.size() ) {
        pRet = new Virole();
        m_lVirole.insert(p_viroleIndex, pRet);
    } else {
        fLogError(QString("Bad index : %1 > %2").arg(p_viroleIndex).arg(p_viroleIndex));
    }
    return pRet;
}

void PTankShell::removeVirole(qint32 p_viroleIndex)
{
    if( p_viroleIndex < m_lVirole.size() ) {
        Virole* pV = m_lVirole.at(p_viroleIndex);
        m_lVirole.removeAt(p_viroleIndex);
        delete pV;
    } else {
        fLogError(QString("Bad index : %1 >= %2").arg(p_viroleIndex).arg(p_viroleIndex));
    }
}

Virole* PTankShell::getVirole(qint32 p_viroleIndex) const
{
    Virole* pRet = nullptr;
    if( p_viroleIndex < m_lVirole.size() ) {
        pRet = m_lVirole.at(p_viroleIndex);
    } else {
        fLogError(QString("Bad index : %1 >= %2").arg(p_viroleIndex).arg(p_viroleIndex));
    }
    return pRet;
}


void PTankShell::computePropertiesFromParameters(void)
{
    // \todo
//    m_genCircularHeightNb;
//    m_genCircularHeightSpacingCm;
//    m_genCircularPerimeterNbMeasure;
//    m_genCircularPerimeterSpacingCmComputed;

}

quint32 PTankShell::getOperatorMode(void) const
{
    return m_operatorMode;
}

void PTankShell::setOperatorMode(quint32 p_value)
{
    m_operatorMode = p_value;
}

quint32 PTankShell::getGeneratrixPerimeterMode(void) const
{
    return m_genGeneratixPerimeterMode;
}

void PTankShell::setGeneratricPerimeterMode(quint32 p_value)
{
    m_genGeneratixPerimeterMode = p_value;
}

quint32 PTankShell::getGeneratrixPerimeterNbFree(void) const
{
    return m_genGeneratixPerimeterNbFree;
}

void PTankShell::setGeneratrixPerimeterNbFree(quint32 p_value)
{
    m_genGeneratixPerimeterNbFree = p_value;
}

quint32 PTankShell::getGeneratrixHeightMode(void) const
{
    return m_genGeneratrixHeightMode;
}

void PTankShell::setGeneratricHeightMode(quint32 p_value)
{
    m_genGeneratrixHeightMode = p_value;
}

quint32 PTankShell::getGeneratrixHeightNbFree(void) const
{
    return m_genGeneratrixHeightNbFree;
}

void PTankShell::setGeneratricHeightNbFree(quint32 p_value)
{
    m_genGeneratrixHeightNbFree = p_value;
}

quint32 PTankShell::getGeneratrixHeightSpacingCm(void) const
{
    return m_genGeneratrixHeightSpacingCm;
}

void PTankShell::setGeneratricHeightSpacingCm(quint32 p_value)
{
    m_genGeneratrixHeightSpacingCm = p_value;
}

quint32 PTankShell::getGeneratrixHeightNbVirole(void) const
{
    return m_genGeneratrixHeightNbVirole;
}

void PTankShell::setGeneratricHeightNbVirole(quint32 p_value)
{
    m_genGeneratrixHeightNbVirole = p_value;
}

quint32 PTankShell::getGeneratrixHeightNbMeasureByVirole(void) const
{
    return m_genGeneratrixHeightNbMeasureByVirole;
}

void PTankShell::setGeneratricHeightNbMeasureByVirole(quint32 p_value)
{
    m_genGeneratrixHeightNbMeasureByVirole = p_value;
}

bool PTankShell::getGeneratrixHeightViroleDT94(void) const
{
    return m_genGeneratrixHeightViroleDT94;
}

void PTankShell::setGeneratricHeightViroleDT94(bool p_value)
{
    m_genGeneratrixHeightViroleDT94 = p_value;
}


quint32 PTankShell::getGeneratrixPerimeterNb(void) const
{
    return m_genGeneratrixPerimeterNb;
}

quint32 PTankShell::getGeneratrixPerimeterSpacingCm(void) const
{
    return m_genGeneratrixPerimeterSpacingCm;
}

quint32 PTankShell::getGeneratrixHeightNbMeasure(void) const
{
    return m_genGeneratrixHeightNbMeasure;
}

quint32 PTankShell::getGeneratrixHeightSpacingCmComputed(void) const
{
    return m_genGeneratrixHeightSpacingCmComputed;
}


quint32 PTankShell::getCircularHeightMode(void) const
{
    return m_genCircularHeightMode;
}

void PTankShell::setCircularHeightMode(quint32 p_value)
{
    m_genCircularHeightMode = p_value;
}

quint32 PTankShell::getCircularHeightNbFree(void) const
{
    return m_genCircularHeightNbFree;
}

void PTankShell::setCircularHeightNbFree(quint32 p_value)
{
    m_genCircularHeightNbFree = p_value;
}

quint32 PTankShell::getCircularHeightNbVirole(void) const
{
    return m_genCircularHeightNbVirole;
}

void PTankShell::setCircularHeightNbVirole(quint32 p_value)
{
    m_genCircularHeightNbVirole = p_value;
}

quint32 PTankShell::getCircularHeightNbMeasureByVirole(void) const
{
    return m_genCircularHeightNbMeasureByVirole;
}

void PTankShell::setCircularHeightNbMeasureByVirole(quint32 p_value)
{
    m_genCircularHeightNbMeasureByVirole = p_value;
}

bool PTankShell::getCircularHeightViroleDT94(void) const
{
    return m_genCircularHeightViroleDT94;
}

void PTankShell::setCircularHeightViroleDT94(bool p_value)
{
    m_genCircularHeightViroleDT94 = p_value;
}

quint32 PTankShell::getCircularPerimeterMode(void) const
{
    return m_genCircularPerimeterMode;
}

void PTankShell::setCircularPerimeterMode(quint32 p_value)
{
    m_genCircularPerimeterMode = p_value;
}

quint32 PTankShell::getCircularPerimeterNbFree(void) const
{
    return m_genCircularPerimeterNbFree;
}

void PTankShell::setCircularPerimeterNbFree(quint32 p_value)
{
    m_genCircularPerimeterNbFree = p_value;
}

quint32 PTankShell::getCircularPerimeterSpacingCm(void) const
{
    return m_genCircularPerimeterSpacingCm;
}

void PTankShell::setCircularPerimeterSpacingCm(quint32 p_value)
{
    m_genCircularPerimeterSpacingCm = p_value;
}


quint32 PTankShell::getCircularHeightNb(void) const
{
    return m_genCircularHeightNb;
}

quint32 PTankShell::getCircularHeightSpacingCm(void) const
{
    return m_genCircularHeightSpacingCm;
}

quint32 PTankShell::getCircularPerimeterNbMeasure(void) const
{
    return m_genCircularPerimeterNbMeasure;
}

quint32 PTankShell::getCircularPerimeterSpacingCmComputed(void) const
{
    return m_genCircularPerimeterSpacingCmComputed;
}


// -- XmlProject
bool PTankShell::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Piece::customLoadParam(p_pParent);

    // read list m_lVirole;
    deleteViroles();
    // find element list
    QDomElement eList = p_pParent->firstChildElement("listVirole");
    if( !eList.isNull() ) {
        QDomElement eVirole= eList.firstChildElement("virole");
        while( !eVirole.isNull() && bRet ) {
            // load zoning
            Virole * virole = new Virole();
            m_lVirole.append(virole);
            bRet &= virole->customLoadParam(&eVirole);
            // next iteration
            eVirole = eVirole.nextSiblingElement("virole");
        }
    } else {
        fLogError("missing tag 'listVirole'");
        bRet = false;
    }

    bRet &= XML_READ_UNIQUE(quint32, m_operatorMode);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratixPerimeterMode);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratixPerimeterNbFree);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightMode);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightNbFree);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightSpacingCm);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightNbVirole);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightNbMeasureByVirole);
    bRet &= XML_READ_UNIQUE(bool, m_genGeneratrixHeightViroleDT94);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixPerimeterNb);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixPerimeterSpacingCm);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightNbMeasure);
    bRet &= XML_READ_UNIQUE(quint32, m_genGeneratrixHeightSpacingCmComputed);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularHeightMode);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularHeightNbFree);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularHeightNbVirole);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularHeightNbMeasureByVirole);
    bRet &= XML_READ_UNIQUE(bool, m_genCircularHeightViroleDT94);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularPerimeterMode);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularPerimeterNbFree);
    bRet &= XML_READ_UNIQUE(quint32, m_genCircularPerimeterSpacingCm);

    return bRet;
}

bool PTankShell::customLoadDefaultParam()
{
    bool bRet = Piece::customLoadDefaultParam();

    deleteViroles();

    // -- Parameters (generals / common)
    m_operatorMode = OPERATING_GENERATRIX;

    // -- Generatrix Operating Mode
    // Parameters
    m_genGeneratixPerimeterMode = GENERATRIX_P_DT94;    ///< value in GeneratrixPerimeterMode
    m_genGeneratixPerimeterNbFree = 0;
    m_genGeneratrixHeightMode = GENERATRIX_H_VIROLE;      ///< value in GeneratrixHeightMode
    m_genGeneratrixHeightNbFree = 0;
    m_genGeneratrixHeightSpacingCm = 50;
    m_genGeneratrixHeightNbVirole = 0;
    m_genGeneratrixHeightNbMeasureByVirole = 0;
    m_genGeneratrixHeightViroleDT94 = true;   ///< if false, use m_genGeneratrixHeightNbMeasureByVirole

    // Properties
    m_genGeneratrixPerimeterNb = 0;
    m_genGeneratrixPerimeterSpacingCm = 0;
    m_genGeneratrixHeightNbMeasure = 0;
    m_genGeneratrixHeightSpacingCmComputed = 0;

    // -- Circular Operating Mode
    // Parameters
    m_genCircularHeightMode = CIRCULAR_H_VIROLE;        ///< value in GeneratrixHeightMode
    m_genCircularHeightNbFree = 0;
    m_genCircularHeightNbVirole = 0;
    m_genCircularHeightNbMeasureByVirole = 0;
    m_genCircularHeightViroleDT94 = true;   ///< if false, use m_genCircularHeightNbMeasureByVirole
    m_genCircularPerimeterMode = CIRCULAR_P_SPACING;     ///< value in CircularPerimeterMode
    m_genCircularPerimeterNbFree = 0;
    m_genCircularPerimeterSpacingCm = 50;

    return bRet;
}

bool PTankShell::customApplyParam()
{
    bool bRet = Piece::customApplyParam();
    computePropertiesFromParameters();
    return bRet;
}

bool PTankShell::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Piece::customSaveParam(p_pDoc, p_pParent);

    // --  write list of viroles
    QDomElement eList = p_pParent->firstChildElement("listVirole");
    if( eList.isNull() ) {
        // Parameter doesn't exist, make it !
        eList = p_pDoc->createElement("listVirole");
        p_pParent->appendChild(eList);
    } else {
        // Parameter found, change its value with a new element
        eList.clear();
    }

    for(auto i : qAsConst(m_lVirole)) {
        QDomElement eVirole = p_pDoc->createElement("virole");
        i->customSaveParam(p_pDoc, &eVirole);
        eList.appendChild(eVirole);
    }

    bRet &= XML_WRITE_UNIQUE(m_operatorMode);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratixPerimeterMode);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratixPerimeterNbFree);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightMode);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightNbFree);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightSpacingCm);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightNbVirole);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightNbMeasureByVirole);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightViroleDT94);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixPerimeterNb);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixPerimeterSpacingCm);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightNbMeasure);
    bRet &= XML_WRITE_UNIQUE(m_genGeneratrixHeightSpacingCmComputed);
    bRet &= XML_WRITE_UNIQUE(m_genCircularHeightMode);
    bRet &= XML_WRITE_UNIQUE(m_genCircularHeightNbFree);
    bRet &= XML_WRITE_UNIQUE(m_genCircularHeightNbVirole);
    bRet &= XML_WRITE_UNIQUE(m_genCircularHeightNbMeasureByVirole);
    bRet &= XML_WRITE_UNIQUE(m_genCircularHeightViroleDT94);
    bRet &= XML_WRITE_UNIQUE(m_genCircularPerimeterMode);
    bRet &= XML_WRITE_UNIQUE(m_genCircularPerimeterNbFree);
    bRet &= XML_WRITE_UNIQUE(m_genCircularPerimeterSpacingCm);

    return bRet;
}


// -- operator
PTankShell & PTankShell::operator= (const PTankShell & p_value)
{
    if( this != &p_value ) {
        Piece::operator=(p_value);
        deleteViroles();
        m_operatorMode = p_value.getOperatorMode();
        m_genGeneratixPerimeterMode = p_value.getGeneratrixPerimeterMode();
        m_genGeneratixPerimeterNbFree = p_value.getGeneratrixPerimeterNbFree();
        m_genGeneratrixHeightMode = p_value.getGeneratrixHeightMode();
        m_genGeneratrixHeightNbFree = p_value.getGeneratrixHeightNbFree();
        m_genGeneratrixHeightSpacingCm = p_value.getGeneratrixHeightSpacingCm();
        m_genGeneratrixHeightNbVirole = p_value.getGeneratrixHeightNbVirole();
        m_genGeneratrixHeightNbMeasureByVirole = p_value.getGeneratrixHeightNbMeasureByVirole();
        m_genGeneratrixHeightViroleDT94 = p_value.getGeneratrixHeightViroleDT94();
        m_genGeneratrixPerimeterNb = p_value.getGeneratrixPerimeterNb();
        m_genGeneratrixPerimeterSpacingCm = p_value.getGeneratrixPerimeterSpacingCm();
        m_genGeneratrixHeightNbMeasure = p_value.getGeneratrixHeightNbMeasure();
        m_genGeneratrixHeightSpacingCmComputed = p_value.getGeneratrixHeightSpacingCmComputed();
        m_genCircularHeightMode = p_value.getCircularHeightMode();
        m_genCircularHeightNbFree = p_value.getCircularHeightNbFree();
        m_genCircularHeightNbVirole = p_value.getCircularHeightNbVirole();
        m_genCircularHeightNbMeasureByVirole = p_value.getCircularHeightNbMeasureByVirole();
        m_genCircularHeightViroleDT94 = p_value.getCircularHeightViroleDT94();
        m_genCircularPerimeterMode = p_value.getCircularPerimeterMode();
        m_genCircularPerimeterNbFree = p_value.getCircularPerimeterNbFree();
        m_genCircularPerimeterSpacingCm = p_value.getCircularPerimeterSpacingCm();
        m_genCircularHeightNb = p_value.getCircularHeightNb();
        m_genCircularHeightSpacingCm = p_value.getCircularHeightSpacingCm();
        m_genCircularPerimeterNbMeasure = p_value.getCircularPerimeterNbMeasure();
        m_genCircularPerimeterSpacingCmComputed = p_value.getCircularPerimeterSpacingCmComputed();
    }
    return (*this);
}

bool PTankShell::operator== (const PTankShell & p_value)
{
    return Piece::operator==(p_value) &&
            (m_operatorMode == p_value.getOperatorMode()) &&
            (m_genGeneratixPerimeterMode == p_value.getGeneratrixPerimeterMode()) &&
            (m_genGeneratixPerimeterNbFree == p_value.getGeneratrixPerimeterNbFree()) &&
            (m_genGeneratrixHeightMode == p_value.getGeneratrixHeightMode()) &&
            (m_genGeneratrixHeightNbFree == p_value.getGeneratrixHeightNbFree()) &&
            (m_genGeneratrixHeightSpacingCm == p_value.getGeneratrixHeightSpacingCm()) &&
            (m_genGeneratrixHeightNbVirole == p_value.getGeneratrixHeightNbVirole()) &&
            (m_genGeneratrixHeightNbMeasureByVirole == p_value.getGeneratrixHeightNbMeasureByVirole()) &&
            (m_genGeneratrixHeightViroleDT94 == p_value.getGeneratrixHeightViroleDT94()) &&
            (m_genGeneratrixPerimeterNb == p_value.getGeneratrixPerimeterNb()) &&
            (m_genGeneratrixPerimeterSpacingCm == p_value.getGeneratrixPerimeterSpacingCm()) &&
            (m_genGeneratrixHeightNbMeasure == p_value.getGeneratrixHeightNbMeasure()) &&
            (m_genGeneratrixHeightSpacingCmComputed == p_value.getGeneratrixHeightSpacingCmComputed()) &&
            (m_genCircularHeightMode == p_value.getCircularHeightMode()) &&
            (m_genCircularHeightNbFree == p_value.getCircularHeightNbFree()) &&
            (m_genCircularHeightNbVirole == p_value.getCircularHeightNbVirole()) &&
            (m_genCircularHeightNbMeasureByVirole == p_value.getCircularHeightNbMeasureByVirole()) &&
            (m_genCircularHeightViroleDT94 == p_value.getCircularHeightViroleDT94()) &&
            (m_genCircularPerimeterMode == p_value.getCircularPerimeterMode()) &&
            (m_genCircularPerimeterNbFree == p_value.getCircularPerimeterNbFree()) &&
            (m_genCircularPerimeterSpacingCm == p_value.getCircularPerimeterSpacingCm()) &&
            (m_genCircularHeightNb == p_value.getCircularHeightNb()) &&
            (m_genCircularHeightSpacingCm == p_value.getCircularHeightSpacingCm()) &&
            (m_genCircularPerimeterNbMeasure == p_value.getCircularPerimeterNbMeasure()) &&
            (m_genCircularPerimeterSpacingCmComputed == p_value.getCircularPerimeterSpacingCmComputed());
}

Piece *PTankShell::getCopy() const
{
    return  new PTankShell(*this);
}

void PTankShell::deleteViroles()
{
    for(auto i : qAsConst(m_lVirole)) {
        delete i;
    }
    m_lVirole.clear();
}
