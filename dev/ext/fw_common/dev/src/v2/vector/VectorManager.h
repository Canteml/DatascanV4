/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorManager.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
class VectorStd;

Q_DECLARE_OPAQUE_POINTER(VectorStd*)
Q_DECLARE_METATYPE(VectorStd*)

// -- Qt includes
#include <QObject>
#include <qqml.h>

class VectorManager : public QObject, public XmlSettingsLoggable {
    Q_OBJECT

    QML_ELEMENT

public :
    ~VectorManager();

    static VectorManager * makeInstance(QObject *parent);
    static VectorManager * getInstance();

    // -- Manager
    bool init();
    bool quit();

    void makeVectorStd();   ///< p_deviceType in IntComMsg::DEVICE_TYPE

public slots :
    VectorStd * getVectorStd();

    
protected :
    // -- Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
        
private :

    // -- Singleton design
    VectorManager(QObject *parent);
    static VectorManager * m_pMyInstance;

    // -- Behaviour
    VectorStd* m_pVectorStd;
};
