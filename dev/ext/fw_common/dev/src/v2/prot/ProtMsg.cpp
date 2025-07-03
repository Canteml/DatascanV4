/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ProtMsg.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/prot/ProtMsg.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- ProtMsg

const QString ProtMsg::PROD_NAME[PROD_MAX_ID] = {"UNDEF",
                                                 "UT POLE",
                                                 "ALTISCAN-V2",
                                                 "ROVER",
                                                 "ROVER IP68",
                                                 "ROVER CLEANER",
                                                 "ALTISCAN",
                                                 "PANDA VOL3",
                                                 "MultiScan"};

ProtMsg::ProtMsg()
{
    m_len = 0;
    m_deviceAd    = DEVICE_AD_MPC;
    m_msgType     = MSG_TYPE_UNDEF;     ///< value in MSG_TYPE
    m_deviceFunc  = DEVICE_FUNC_UNDEF;      ///< value in MSG_GRP
    m_msgObj      = MSG_OBJ_UNDEF;     ///< value function to type and group to specify the object
}

bool ProtMsg::isAdControlPanel() const
{
    return (m_deviceAd != DEVICE_AD_MPC) && (m_deviceAd < DEVICE_AD_SECONDARY_CONTROL_PANEL);
}

bool ProtMsg::isParamDevice() const
{
    return !(m_msgObj & 0b10000000);
}

bool ProtMsg::isParamLogical() const
{
    return (m_msgObj & 0b10000000);
}

QString ProtMsg::prodName(quint8 p_prodId)
{
    if( p_prodId<PROD_MAX_ID ) {
        return PROD_NAME[p_prodId];
    }
    return "ERROR PROD ID";
}
