/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncGeneralAuth.h
# Author      : Thierry Lechevallier
# Date        : 21/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncGeneralAuth.h>
#include <winsock.h>

VectorFuncGeneralAuth::VectorFuncGeneralAuth() : XmlProject("VectorFuncGeneralAuth")
{
    m_paramDProdId = 0;
    m_paramDDeviceAd = 0;
    m_paramDProdUid = "#";
    m_paramDVerHw = "0.0-DevHw";
    m_paramDVerSw = "0.0-DevSw";
    m_paramDDateBuild = QDateTime::currentSecsSinceEpoch();
    m_paramDDateRev = QDateTime::currentSecsSinceEpoch();

    for(int i = 0 ; i < 3 ; i++) {
        m_alerts[i] = QString();
    }

}

VectorFuncGeneralAuth::~VectorFuncGeneralAuth()
{

}

// -- Inherited from XmlProject
bool VectorFuncGeneralAuth::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;

    QString lFuncEnable;
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, lFuncEnable, "");
    QStringList slFunc = lFuncEnable.split(",");
    m_lFunctions.clear();
    for(int i=0; i<slFunc.size(); i++) {
        bool bOk;
        qint32 f = slFunc.at(i).toInt(&bOk);
        if( bOk ) {
            m_lFunctions.append(f);
        }
    }

    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_paramDProdId, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_paramDDeviceAd, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_paramDProdUid, "#");
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_paramDVerHw, "0.0-DevHw");
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_paramDVerSw, "0.0-DevSw");
    bRet &= XML_READ_UNIQUE_DEFAULT(qint64, m_paramDDateBuild, QDateTime::currentSecsSinceEpoch());
    bRet &= XML_READ_UNIQUE_DEFAULT(qint64, m_paramDDateRev, QDateTime::currentSecsSinceEpoch());
    return bRet;
}

bool VectorFuncGeneralAuth::customLoadDefaultParam()
{
    bool bRet = true;
    m_lFunctions.clear();
    m_paramDProdId = 0;
    m_paramDDeviceAd = 0;
    m_paramDProdUid = "#";
    m_paramDVerHw = "0.0-DevHw";
    m_paramDVerSw = "0.0-DevSw";
    m_paramDDateBuild = QDateTime::currentSecsSinceEpoch();
    m_paramDDateRev = QDateTime::currentSecsSinceEpoch();
    return bRet;
}

bool VectorFuncGeneralAuth::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncGeneralAuth::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString lFuncEnable;
    for(int i=0; i<m_lFunctions.size(); i++) {
        lFuncEnable.append(QString::number(m_lFunctions.at(i)));
        if( i<(m_lFunctions.size()-1) ) {
            lFuncEnable.append(",");
        }
    }
    bRet &= XML_WRITE_UNIQUE(lFuncEnable);
    bRet &= XML_WRITE_UNIQUE(m_paramDProdId);
    bRet &= XML_WRITE_UNIQUE(m_paramDDeviceAd);
    bRet &= XML_WRITE_UNIQUE(m_paramDProdUid);
    bRet &= XML_WRITE_UNIQUE(m_paramDVerHw);
    bRet &= XML_WRITE_UNIQUE(m_paramDVerSw);
    bRet &= XML_WRITE_UNIQUE(m_paramDDateBuild);
    bRet &= XML_WRITE_UNIQUE(m_paramDDateRev);
    return bRet;
}
