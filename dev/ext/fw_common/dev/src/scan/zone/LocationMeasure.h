/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : LocationMeasure.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <xml/XmlProject.h>

// -- Qt includes
#include <QPointF>


// ----------------------------------------------------------------------------
// -- Class LocationMeasure

/**
 * @brief the LocationMeasure class
 *
 * A Mesure is localised in a location.
 *
 * improvement : use template type for samples instead of qint16
 *
 */
class LocationMeasure : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit LocationMeasure(const QString& p_className = "LocationMeasure");
    virtual ~LocationMeasure();

    void setSamples(QVector<qint16> * p_pSample);
    void copySamples(const QVector<qint16> & p_pSample);
    const QVector<qint16> * getSamples(void);
    void removeSamples();

    QPointF getPosition(void) const;
    void setPosition(const QPointF & p_position);

    void setTime(qint64 p_timeMs);
    void setTimeToNow();
    qint64 getTime();
    QString getTimeString();

    qreal getValue(void) const;
    void setValue(qreal p_value);

    void computeValueFromSample(void);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    qreal m_value;              ///< value computed from samples
    QPointF m_position;         ///< position in the Location
    qint64 m_timeMs;            ///< value in ms from epoc time (local clock)
    QVector<qint16> * m_pSample; ///< pointer to the array of samples. Can be set without copy

    // -- Agregation
    // \todo : réglage mesure
};


