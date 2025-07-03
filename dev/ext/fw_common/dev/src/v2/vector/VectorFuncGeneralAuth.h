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

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>

// -- Qt includes
#include <QDateTime>

/**
 * @brief The VectorFuncGeneralAuth class
 *
 * Implement the function general.
 * This function is always implemented by a Vector.
 *
 */

class VectorFuncGeneralAuth : public XmlProject {

public :
    VectorFuncGeneralAuth();
    ~VectorFuncGeneralAuth();

    // -- Function General
    qint32 m_paramDProdId;      ///< product ID, value in PROD_ID. Device discovered in case of MPC, Device made in case of simu
    qint32 m_paramDDeviceAd;    ///< Valeur de l’adresse d’un DEVICE dans un produit
    QString m_paramDProdUid;    ///< Identifiant unique propre au produit. Chaîne de 16 caractères se terminant par le caractère nul (‘\0’). Joue le rôle de numéro de série.
    QString m_paramDVerHw;      ///< Numéro de version du matériel. string [12 bytes] se terminant par le caractère nul ‘\0’
    QString m_paramDVerSw;      ///< Numéro de version du firmware. string [12 bytes] se terminant par le caractère nul ‘\0’
    qint64 m_paramDDateBuild;   ///< Date de construction de l’appareil. [8 bytes] : epoch time en secondes (int64)
    qint64 m_paramDDateRev;     ///< Date de révision de l’appareil. [8 bytes] : epoch time en secondes (int64)
    QList<quint32> m_lFunctions;            ///< functions managed by the device. Discovered in case of MPC, set in case of simu

    QString m_alerts[3];

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};
