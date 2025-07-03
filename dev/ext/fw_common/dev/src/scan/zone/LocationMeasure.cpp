/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : LocationMeasure.cpp
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/zone/LocationMeasure.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- Class LocationMeasure


LocationMeasure::LocationMeasure(const QString& p_className) : XmlProjectLoggable(p_className)
{
    m_value = 0;
    m_position = QPointF(0,0);
    m_timeMs = 0;
    m_pSample = new QVector<qint16>();
}

LocationMeasure::~LocationMeasure()
{
    if( nullptr != m_pSample ) {
        delete m_pSample;
        m_pSample = nullptr;
    }
}


void LocationMeasure::setSamples(QVector<qint16> * p_pSample)
{
    // delete old samples
    if( nullptr != m_pSample ) {
        delete m_pSample;
        m_pSample = nullptr;
    }
    // affectation
    m_pSample = p_pSample;
}

void LocationMeasure::copySamples(const QVector<qint16> & p_pSample)
{
    removeSamples();
    // copy
    (*m_pSample) = p_pSample;
}

const QVector<qint16> * LocationMeasure::getSamples(void)
{
    return m_pSample;
}

void LocationMeasure::removeSamples()
{
    if( nullptr != m_pSample ) {
        m_pSample->clear();
    }
}


QPointF LocationMeasure::getPosition(void) const
{
    return m_position;
}

void LocationMeasure::setPosition(const QPointF & p_position)
{
    m_position = p_position;
}


void LocationMeasure::setTime(qint64 p_timeMs)
{
    m_timeMs = p_timeMs;
}

void LocationMeasure::setTimeToNow()
{
    m_timeMs = QDateTime::currentMSecsSinceEpoch();
}

qint64 LocationMeasure::getTime()
{
    return m_timeMs;
}

QString LocationMeasure::getTimeString()
{
    return QDateTime::fromMSecsSinceEpoch(m_timeMs).toString("dd/MM/yy hh:mm:ss.zzz");
}


qreal LocationMeasure::getValue(void) const
{
    return m_value;
}

void LocationMeasure::setValue(qreal p_value)
{
    m_value = p_value;
}


void LocationMeasure::computeValueFromSample(void)
{
    // \todo (here ?)
}


// -- XmlProject
bool LocationMeasure::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(qreal, m_value);
    bRet &= XML_READ_UNIQUE(QPointF, m_position);
    bRet &= XML_READ_UNIQUE(qint64, m_timeMs);

    // samples
    QDomElement eSamples = p_pParent->firstChildElement("samples");
    if( eSamples.isNull() ) {
        fLogError("tag 'samples' bot found");
        bRet = false;
    } else {
        removeSamples();
        // convert string to bytes : "FA" => 0xFA
        QByteArray baSamples = QByteArray::fromHex(eSamples.toText().data().toLatin1());
        // samples are words of 2 bytes
        int sampleSize = baSamples.size();
        char* pSamples = baSamples.data();
        m_pSample->resize(sampleSize);
        for( int i=0; i<sampleSize; i+=2 ) {
            (*m_pSample)[i] = (*(qint16*)(&pSamples[i]));
        }
    }
    return bRet;
}

bool LocationMeasure::customLoadDefaultParam()
{
    m_value = 0;
    m_position = QPointF(0,0);
    m_timeMs = 0;
    removeSamples();
    return true;
}

bool LocationMeasure::customApplyParam()
{
    return true;
}

bool LocationMeasure::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_value);
    bRet &= XML_WRITE_UNIQUE(m_position);
    bRet &= XML_WRITE_UNIQUE(m_timeMs);

    // make QString with samples
    QString samples;
    int sampleSize = m_pSample->size();
    QByteArray ba;
    ba.resize(2);
    for(int i = 0; i<sampleSize; i++) {
        ba.setRawData((char *)(&(*m_pSample)[i]), 2);
        samples.append(ba.toHex());
    }

    QDomElement eSample = p_pParent->firstChildElement("samples");
    if( eSample.isNull() ) {
        eSample = p_pDoc->createElement("samples");
        p_pParent->appendChild(eSample);
    } else {
        QDomElement eNewSample = p_pDoc->createElement("samples");
        p_pParent->replaceChild(eNewSample, eSample);
        eSample = eNewSample;
    }
    eSample.setNodeValue(samples);

    return bRet;
}
