/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Probe.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/type/Calibrate.h>
class Calibrate;

// -- Qt includes
#include <QPointF>
#include <QRectF>
#include <QSizeF>

// ----------------------------------------------------------------------------
// -- Class VectorParam

/**
 * @brief the VectorParam class
 *
 */
class VectorParam : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit VectorParam();
    explicit VectorParam(const VectorParam & p_value);  ///< copy constructor
    virtual ~VectorParam();

    virtual quint32 getVectorType(void) const = 0;      ///< value in Vector::VectorType

    virtual VectorParam* copy() const = 0;

    quint32 getId(void) const;
    void setId(quint32 p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    VectorParam & operator= (const VectorParam & p_value);
    bool operator== (const VectorParam & p_value);
    bool operator!= (const VectorParam & p_value);

protected :
    quint32 m_ID;           ///< used as reference if data base are available (0 = invalid ID)
};

// ----------------------------------------------------------------------------
// -- Class VectorCalibrate

// \todo : has a VectorSettings (use for calibrate) and a CalibrateReport

class VectorCalibrate : public Calibrate
{
public :
    explicit VectorCalibrate();
    virtual ~VectorCalibrate();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    VectorCalibrate &operator= (const VectorCalibrate & p_value);
    bool operator== (const VectorCalibrate & p_value);
    bool operator!= (const VectorCalibrate & p_value);
};

// ----------------------------------------------------------------------------
// -- Class Vector

/**
 * @brief the Vector class
 */
 
class Vector : public QObject, public XmlProjectLoggable
{
    Q_OBJECT

    // \todo Q_PROPERTY for pos
public :

    // -- Type
    enum VectorType {
        TYPE_UNDEF,
        TYPE_UTPOLE,
        TYPE_ALTISCAN,
        TYPE_DRONESCAN
    };

    static QString getVectorTypeName(quint32 e);       ///< return a string with the name of the vector type

    // -- Methods

    explicit Vector(VectorParam * p_pVectorParam, VectorCalibrate * p_pVectorCalibrate);
    virtual ~Vector();

    virtual void init();

    quint32 getVectorType(void) const;   ///< return a value in Vector::VectorType
    QString getVectorName(void) const;   ///< return the name of the vector

    VectorParam* getParam();
    VectorCalibrate* getCalibrate();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Properties
    qreal getX() const;
    void setX(qreal p_value);
    qreal getY() const;
    void setY(qreal p_value);
    QPointF getPos() const;
    void setPos(const QPointF &p_value);

    // -- Composition
    VectorParam *getVectorParam() const;
    void setVectorParam(const VectorParam & p_value);
    VectorCalibrate *getVectorCalibrate() const;
    void setVectorCalibrate(const VectorCalibrate & p_value);

    // -- References
    QString getVectorBrand() const;
    void setVectorBrand(const QString & p_value);
    QString getVectorBrandType() const;
    void setVectorBrandType(const QString & p_value);
    QString getVectorSerialNumber() const;
    void setVectorSerialNumber(const QString & p_value);

    // -- Param scan
    QPointF getVectorProbeOffset() const;
    void setVectorProbeOffset(const QPointF & p_value);
    QRectF getVectorZoneSweeping() const;
    void setZoneSweeping(const QRectF & p_value);
    QSizeF getVectorSize() const;
    void setVectorSize(const QSizeF & p_value);

    // -- Behaviour
    static Vector *makeVector(quint32 p_type);

    // -- operator
    Vector & operator= (const Vector & p_value);
    bool operator== (const Vector & p_value);

protected :
    
    // -- Param constant

    // -- Param References
    QString m_vectorBrand;
    QString m_vectorBrandType;			///< Brand part
    QString m_vectorSerialNumber;

    // -- Param scan
    QPointF m_vectorProbeOffset;    ///< offset of the probe origin. Will be the origin of the swept zone.
    QRectF m_vectorZoneSweeping;    ///< zone swept by the probe, can be null, defined of infiny (-1)
    QSizeF m_vectorSize;       ///< outline of the vector, take care of the origin

    // -- Properties
    QPointF m_position;             ///< value in the Location Coordinates

    // -- Composition
    VectorParam * m_pVectorParam;
    VectorCalibrate * m_pVectorCalibrate;
};



