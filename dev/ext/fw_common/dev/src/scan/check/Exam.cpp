/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Exam.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

// -- Internal includes
#include <scan/check/Exam.h>
#include <scan/structure/Structure.h>
#include <scan/environment/Operator.h>
#include <scan/equipment/Equipment.h>
#include <scan/check/CheckPiece.h>



Exam::Exam() : XmlProjectLoggable("Exam")
{
    m_date = "";
    m_pStructure = nullptr;
    m_pOperator = nullptr;
    m_pEquipment = nullptr;
}

Exam::~Exam()
{    
    removeAllCheckPiece();
    if( nullptr != m_pStructure ) {
        delete m_pStructure;
        m_pStructure = nullptr;
    }
    if( nullptr != m_pOperator ) {
        delete m_pOperator;
        m_pOperator = nullptr;
    }
    if( nullptr != m_pEquipment ) {
        delete m_pEquipment;
        m_pEquipment = nullptr;
    }
}


void Exam::removeAllCheckPiece()
{
    for(auto i: qAsConst(m_lCheckPiece)) {
        delete i;
    }
    m_lCheckPiece.clear();
}

QString Exam::getDateString() const
{
    return m_date;
}

void Exam::setDate(const QDateTime & p_value)
{
    m_date = p_value.toString("hh:mm:ss dd/MM/yyyy");
}

void Exam::setDate(int p_hour, int p_minute, int p_second, int p_day, int p_month, int p_year)
{
    m_date = QString("%1:%2:%3 %4/%5/%6").arg(p_hour).arg(p_minute).arg(p_second).arg(p_day).arg(p_month).arg(p_year);
}

void Exam::setDateToNow(void)
{
    m_date = QDateTime::currentDateTime().toString("hh:mm:ss dd/MM/yyyy");
}

bool Exam::isDateSet() const
{
    return !m_date.isEmpty();
}

QString Exam::getComment() const
{
    return m_comment;
}

void Exam::setComment(const QString &p_value)
{
    m_comment = p_value;
}



// -- XmlProject
bool Exam::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_date);    
    bRet &= XML_READ_UNIQUE(QString, m_comment);
    bRet &= m_pStructure->loadParam(p_pParent);
    bRet &= m_pOperator->loadParam(p_pParent);
    bRet &= m_pEquipment->loadParam(p_pParent);

    // -- list of CheckPiece
    removeAllCheckPiece();
    QDomElement eListCP = p_pParent->firstChildElement("listCkeckPiece");
    if( !eListCP.isNull() ) {
        QDomElement eCP = eListCP.firstChildElement("CheckPiece");
        while(!eCP.isNull() && bRet) {
            CheckPiece * pCP = new CheckPiece();
            bRet &= pCP->customLoadParam(&eCP);
            eCP = eCP.nextSiblingElement("CheckPiece");
        }
    } else {
        fLogError("Fail to find 'listCkeckPiece'");
        bRet = false;
    }
    return bRet;
}

bool Exam::customLoadDefaultParam()
{
    m_date = "";
    m_comment = "";
    if( m_pStructure != nullptr ) {
        delete m_pStructure;
        m_pStructure = nullptr;
    }
    if( m_pOperator != nullptr ) {
        delete m_pOperator;
        m_pOperator = nullptr;
    }
    if( m_pEquipment != nullptr ) {
        delete m_pEquipment;
        m_pEquipment = nullptr;
    }
    removeAllCheckPiece();

    return true;
}

bool Exam::customApplyParam()
{
    return true;
}

bool Exam::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_date);
    bRet &= XML_WRITE_UNIQUE(m_comment);
    bRet &= m_pStructure->saveParam(p_pDoc, p_pParent);
    bRet &= m_pOperator->saveParam(p_pDoc, p_pParent);
    bRet &= m_pEquipment->saveParam(p_pDoc, p_pParent);

    // -- list of CheckPiece
    QDomElement eListCP = p_pParent->firstChildElement("listCkeckPiece");
    if( eListCP.isNull() ) {
        // Parameter doesn't exist, make it !
        eListCP = p_pDoc->createElement("listCkeckPiece");
        p_pParent->appendChild(eListCP);
    } else {
        QDomElement newE = p_pDoc->createElement("listCkeckPiece");
        p_pParent->replaceChild(eListCP, newE);
        eListCP = newE;
    }
    // add elements of the list
    for(auto i : qAsConst(m_lCheckPiece)) {
        bRet &= i->saveParam(p_pDoc, &eListCP);
        if( !bRet ) {
            fLogError("Fail to write element 'CheckPiece'");
            break;  // exit if an error occured
        }
    }

    return bRet;
}
