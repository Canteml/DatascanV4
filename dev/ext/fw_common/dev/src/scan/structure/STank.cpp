/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : STank.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/structure/STank.h>
#include <scan/structure/PTankRoof.h>
#include <scan/structure/PTankShell.h>

// -- Qt includes
#include <QtMath>

// ----------------------------------------------------------------------------
// -- Class STankFeature


STankFeature::STankFeature() : XmlProjectLoggable("STankFeature")
{
    init();
}

STankFeature::STankFeature(const STankFeature &p_value) : QObject(), XmlProjectLoggable("STankFeature")
{
    init();
    (*this) = p_value;
}

STankFeature::~STankFeature()
{

}


// -- Parameters

QString STankFeature::getTankSerialNumber() const
{
    return m_tankSerialNumber;
}

void STankFeature::setTankSerialNumber(const QString & p_value)
{
    m_tankSerialNumber = p_value;
}

qreal STankFeature::getTankHeight(void) const
{
    return m_tankHeightCm;
}

void STankFeature::setTankHeight(qreal p_value)
{
    m_tankHeightCm = p_value;
}


qreal STankFeature::getTankDiameterCm(void) const
{
    return m_tankDiameterCm;
}

qreal STankFeature::getTankPerimeterCm(void) const
{
    return m_tankDiameterCm*M_PI; // perimeter = PI x D
}

void STankFeature::setTankDiameterCm(qreal p_value)
{
    m_tankDiameterCm = p_value;
}

void STankFeature::setTankPerimeterCm(qreal p_value)
{
    m_tankDiameterCm = p_value / M_PI;
}

// -- XmlProject
bool STankFeature::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_tankSerialNumber);
    bRet &= XML_READ_UNIQUE(qreal, m_tankHeightCm);
    bRet &= XML_READ_UNIQUE(qreal, m_tankDiameterCm);
    return bRet;
}

bool STankFeature::customLoadDefaultParam()
{
    init();
    return true;
}

bool STankFeature::customApplyParam()
{
    return true;
}

bool STankFeature::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_tankSerialNumber);
    bRet &= XML_WRITE_UNIQUE(m_tankHeightCm);
    bRet &= XML_WRITE_UNIQUE(m_tankDiameterCm);
    return bRet;
}


// -- operator
STankFeature & STankFeature::operator= (const STankFeature & p_value)
{
    if( this != (&p_value) ) {
        m_tankSerialNumber = p_value.getTankSerialNumber();
        m_tankHeightCm = p_value.getTankHeight();
        m_tankDiameterCm = p_value.getTankDiameterCm();
    }
    return (*this);
}

bool STankFeature::operator== (const STankFeature & p_value)
{
    return  (m_tankSerialNumber == p_value.getTankSerialNumber()) &&
    (m_tankHeightCm == p_value.getTankHeight()) &&
    (m_tankDiameterCm == p_value.getTankDiameterCm());
}

STankFeature * STankFeature::getCopy() const
{
    return new STankFeature(*this);
}

void STankFeature::init()
{
    m_tankSerialNumber = "";
    m_tankHeightCm = 20000;
    m_tankDiameterCm = 20000;
}



// ----------------------------------------------------------------------------
// -- Class STank

STank::STank() : Structure(Structure::STR_TANK)
{
    init();
}

STank::STank(const STank & p_value) : Structure(Structure::STR_TANK)
{
    init();
    (*this) = p_value;
}

STank::~STank()
{
    if( nullptr == m_pSTankFeature ) {
        delete m_pSTankFeature;
        m_pSTankFeature = nullptr;
    }
}


// -- Parameters
STankFeature * STank::getTankFeature() const
{
    return m_pSTankFeature;
}

void STank::setTankFeature(const STankFeature & p_value)
{
    (*m_pSTankFeature) = p_value;
}


PTankRoof * STank::getRoof(void) const
{
    return m_pPTankRoof;
}

void STank::setRoof(PTankRoof * p_roof)
{
    removeRoof();
    if( nullptr != p_roof ) {
        m_pPTankRoof = p_roof;
    } else {
        m_pPTankRoof = new PTankRoof();
    }
    m_lPieces.append(m_pPTankRoof);
}

void STank::removeRoof()
{
    if( nullptr != m_pPTankRoof ) {
        m_lPieces.removeAll(m_pPTankRoof);
        delete m_pPTankRoof;
        m_pPTankRoof = nullptr;
    }
}


PTankShell * STank::getShell(void) const
{
    return m_pPTankShell;
}

void STank::setShell(PTankShell * p_shell)
{
    removeShell();
    if( nullptr != p_shell ) {
        m_pPTankShell = p_shell;
    } else {
        m_pPTankShell = new PTankShell();
    }
    m_lPieces.append(m_pPTankShell);
}

void STank::removeShell()
{
    if( nullptr != m_pPTankShell ) {
        m_lPieces.removeAll(m_pPTankShell);
        delete m_pPTankShell;
        m_pPTankShell = nullptr;
    }
}

QString STank::getStructName()
{
    QString sRet = Structure::getStructName();
    if( !m_pSTankFeature->getTankSerialNumber().isEmpty() )  {
        sRet += QString(" [%2]").arg(m_pSTankFeature->getTankSerialNumber());
    }
    return sRet;
}


// -- XmlProject
bool STank::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Structure::customLoadParam(p_pParent);
    bRet &= m_pSTankFeature->customLoadParam(p_pParent);
    return bRet;
}

bool STank::customLoadDefaultParam()
{
    bool bRet = Structure::customLoadDefaultParam();
    return bRet;
}

bool STank::customApplyParam()
{
    bool bRet = Structure::customApplyParam();
    m_pSTankFeature->customApplyParam();

    // find pieces
    for(auto i : qAsConst(m_lPieces)) {
        if( i->getPieceType() == Piece::PIE_TANK_ROOF ) {
            m_pPTankRoof = static_cast<PTankRoof *>(i);
        } else if( i->getPieceType() == Piece::PIE_TANK_SHELL ) {
            m_pPTankShell = static_cast<PTankShell *>(i);
        } else {
            fLogError(QString("Piece not used : %1").arg(i->getPieceType()));
        }
    }
    return bRet;
}

bool STank::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Structure::customLoadParam(p_pParent);
    bRet &= m_pSTankFeature->customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


// -- operator
STank & STank::operator= (const STank & p_value)
{
    if( this != (&p_value) ) {
            Structure::operator=(p_value);
            (*m_pSTankFeature) = (*p_value.getTankFeature());
    }
    return (*this);
}

bool STank::operator== (const STank & p_value)
{
    bool bRet = true;
    if( !Structure::operator==(p_value) ) {
        bRet = false;
    } else {
        bRet &= (*m_pSTankFeature) == (*p_value.getTankFeature());
        bRet &= (*m_pPTankRoof) == (*p_value.getRoof());
        bRet &= (*m_pPTankShell) == (*p_value.getShell());
    }
    return bRet;
}

Structure * STank::getCopy() const
{
    return new STank(*this);
}

// -- Composition
void STank::init()
{
    if( nullptr == m_pSTankFeature ) {
        delete m_pSTankFeature;
        m_pSTankFeature = nullptr;
    }
    m_pSTankFeature = new STankFeature();
    connect(m_pSTankFeature, &STankFeature::evTankSerialNumberChanged, this, &STank::requestUpdateStructureName);
    m_pSTankFeature->customLoadDefaultParam();
}

void STank::deletePieces()
{
    m_pPTankRoof = nullptr;
    m_pPTankShell = nullptr;
}
