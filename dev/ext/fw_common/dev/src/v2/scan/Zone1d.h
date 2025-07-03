/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Zone1d.h
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>

// -- Qt Includes
#include <QObject>

/**
 * @brief The Zone1d class
 *
 *
 */
class Zone1d : public QObject, public Loggable {
    Q_OBJECT
public :
    Zone1d();
    ~Zone1d();

    bool isSizeDefined() const;

    qreal getOriginXMm() const;
    void setOriginXMm( qreal p_value);

    qreal getSizeXMm() const;
    void setSizeXMm(qreal p_value);

signals:
    void evChanged();

private :

    qreal m_originXMm;  ///< start of the zone in a Surface1d
    qreal m_sizeXMm;    ///< end of the zone in a Surface1d

};
