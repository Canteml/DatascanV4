/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Environment.h
# Author      : Thierry Lechevallier
# Date        : 28/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>

// -- Qt includes


/**
 * @brief the Environment class
 *
 * Conditions of exam
 *
 */
class Environment : public QObject, public XmlProjectLoggable
{
public :

    enum EnvOperating {
        ENV_NOT_OPERATING,
        ENV_OPERATING
    };

    explicit Environment();
    explicit Environment(const Environment & p_value);
    ~Environment();

    // -- Properties
    void setOperating(quint32 p_value);
    quint32 getOperating() const;

    // -- operators
    bool operator==(const Environment & p_value);
    Environment & operator=(const Environment & p_value);

protected :
    // -- XmlSettings
    bool customLoadParam(QDomElement * p_pParent) override;
    bool customLoadDefaultParam() override;
    bool customApplyParam() override;
    bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :

    // -- Properties
    quint32 m_operating;
};

