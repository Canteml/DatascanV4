/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncLicence.h
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

#pragma once

#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncLicence

/**
 * @brief The VectorFuncLicence class
 */
class VectorFuncLicence : public GenVectorFunc
{

    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DStatusLicenceP, quint8, 1, FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P, 0)

public:

    VectorFuncLicence(VectorStd *p_pVectorStd);
    ~VectorFuncLicence();

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    Q_INVOKABLE void configureNewLicence(QString p_licenceKey);

    Q_INVOKABLE void askForKeyEquipment();
    Q_INVOKABLE void askForStatusLicence();
    Q_INVOKABLE QString getKeyEquipment();

    static const quint8 LICENCE_KEY_LENGTH = 135;

signals:

    void keyEquipmentChanged(QString keyEquipment);

protected:

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)

    QString m_keyEquipment;

};

