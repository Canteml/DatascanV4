/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : StructureProject.cpp
# Author      : Thierry Lechevallier
# Date        : 28/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/structure/StructureProject.h>
#include <scan/structure/STank.h>
#include <scan/structure/SFree.h>

StructureProject::StructureProject() : XmlProjectLoggable("Structure")  // Same name that class "Structure"
{
    m_pStructure = nullptr;
}

StructureProject::~StructureProject()
{
    if( nullptr != m_pStructure ) {
        delete m_pStructure;
        m_pStructure = nullptr;
    }
}

Structure *StructureProject::takeStructure()
{
    Structure * pS = m_pStructure;
    m_pStructure = nullptr;
    return pS;
}

Structure *StructureProject::instanciateStructure(quint32 p_type)
{
    Structure * pRet = nullptr;

    if( Structure::STR_TANK ==  p_type ) {
        pRet = new STank();
    } else if( Structure::STR_FREE ==  p_type ) {
        pRet = new SFree();
    } else {
        Logger::logError("StructureProject::instanciateStructure - No class to use for the type : "+QString::number(p_type));
    }
    return pRet;
}

// -- XmlSettings
bool StructureProject::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    // load as the structure class
    quint32 m_structureType;
    bRet &= XML_READ_UNIQUE(quint32, m_structureType);
    if( !bRet ) {
        fLogError("fail to read structure type in XML");
    } else {
        m_pStructure = instanciateStructure(m_structureType);
        // continue to load the structure :
        if( nullptr != m_pStructure ) {
            m_pStructure->customLoadParam(p_pParent);
        } else {
            fLogError(QString("fail to instanciate type : %1 ").arg(m_structureType));
            bRet = false;
        }
    }
    return bRet;
}

bool StructureProject::customLoadDefaultParam()
{
    return m_pStructure->customLoadDefaultParam();
}

bool StructureProject::customApplyParam()
{
    return m_pStructure->customApplyParam();
}

bool StructureProject::customSaveParam(QDomDocument *, QDomElement *)
{
    fLogWarning("should never be called");
    return false;
}


