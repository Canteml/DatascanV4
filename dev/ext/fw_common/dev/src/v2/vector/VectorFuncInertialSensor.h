/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncInertialSensor.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Qt Includes
#include <QtMath>

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- InertialSensorXYZ

/**
 * @brief The InertialSensorXYZ class
 *
 * Define a class to store Acceleration values
 */
class InertialSensorXYZ {
public :
    InertialSensorXYZ() {
        m_x = 0;
        m_y = 0;
        m_z = 0;
    }
    InertialSensorXYZ(qint16 p_x, qint16 p_y, qint16 p_z) {
        m_x = p_x;
        m_y = p_y;
        m_z = p_z;
    }
    InertialSensorXYZ(const InertialSensorXYZ & p_value) : InertialSensorXYZ(p_value.m_x, p_value.m_y, p_value.m_z ){ }

    qint16 m_x;
    qint16 m_y;
    qint16 m_z;

    bool operator!=(const InertialSensorXYZ& p_value) const {
        return (p_value.m_x != m_x) || (p_value.m_y != m_y) || (p_value.m_z != m_z);
    }
    bool operator==(const InertialSensorXYZ& p_value) const {
        return (p_value.m_x == m_x) && (p_value.m_y == m_y) && (p_value.m_z == m_z);
    }

    qreal getXYAngle() {
        return qRadiansToDegrees(qAtan2(m_y, m_x));
    }

    qreal getXZAngle() {
        return qRadiansToDegrees(qAtan2(m_z, m_x));
    }

    qreal getYZAngle() {
        return qRadiansToDegrees(qAtan2(m_z, m_y));
    }

    InertialSensorXYZ& operator=(const InertialSensorXYZ& p_value) {
        if( this != &p_value ) {
            m_x = p_value.m_x;
            m_y = p_value.m_y;
            m_z = p_value.m_z;
        }
        return *this;
    }
};

Q_DECLARE_METATYPE(InertialSensorXYZ);

// ----------------------------------------------------------------------------
// -- VectorFuncInertialSensor

/**
 * @brief The VectorFuncInertialSensor class
 */
class VectorFuncInertialSensor : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT    

    FUNC_PARAM(DInertialSensor, InertialSensorXYZ, 6, FUNC_INERTIAL_SENSOR_PARAM_D_ACC_X_Y_Z, InertialSensorXYZ(0,0,0))
    FUNC_PARAM(LDirection, qint16, 2, FUNC_INERTIAL_SENSOR_PARAM_L_DIRECTION, 0)
    FUNC_PARAM(LHorizontal, quint8, 1, FUNC_INERTIAL_SENSOR_PARAM_L_HORIZONTAL, 0)

public :

    VectorFuncInertialSensor(VectorStd *p_pVectorStd);
    ~VectorFuncInertialSensor();

    // -- PARAM

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

public slots:

    void refreshDirection();

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
};
