/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ProtMsg.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes

// --- Qt includes
#include <stdint.h>
#include <QString>

// ----------------------------------------------------------------------------
// -- ProtMsg

/**
 * @brief The ProtMsg struct
 *
 * MsgId build (32 bits) :
 * MSG_TYPE (8 bits) | MSG_FUNC (8 bits) | MSG_OBJ (16 bits)
 */
typedef struct ProtMsg {
    ProtMsg();
    bool isparameter() const;    ///< return true if the msgType if MSG_TYPE_PARAM_GET, MSG_TYPE_PARAM_SET or MSG_TYPR_PARAM_VALUE
    bool isAdControlPanel() const;
    bool isParamDevice() const;
    bool isParamLogical() const;

    // -- constants
    static const int32_t COM_MSG_SIZE = 8200;
    static const int32_t COM_HEADER_SIZE = 8;
    static const int32_t COM_PAYLOAD_SIZE = 8192;
    static const int32_t COM_PAYLOAD_SIZE_LEGACY = 4080;
    static const int32_t D4_HEARTBEAT_DELAY_MS = 500;
    static const int32_t D4_TIMEOUT_MS = 1000;
    static const int32_t D4_RETRY_MS = 500;
    static const int32_t D4_ASCAN_PAYLOAD_SIZE = 8192;
    static const uint8_t MAGIC_NUMBER = 0xA5;
    static const uint8_t MSG_ID_UNDEF = 0;
    static const uint8_t DEVICE_AD_MPC = 0;  ///< Address of Master Panel Controle (Datascan)
    static const int32_t DEVICE_AD_SECONDARY_CONTROL_PANEL = 128;
    static const int32_t MSG_OBJ_PARAM_LOGICAL_FLAG = 128;   ///< the first 128 values are reserved for Device, the 128 last for logical parameters (control panels)
    static const int32_t HEARTBEAT_DELAY_MS = 500;  ///< when 2 entities has established a authentified link, they must emit a heartbeat periodically.
    static const int32_t TIMEOUT_MS = 3000;

    // -- attributs
    uint16_t m_len;         ///< payload len
    uint8_t m_deviceAd;     ///< address of the device (physical link)
    uint8_t m_msgType;      ///< value in MSG_TYPE
    uint8_t m_deviceFunc;   ///< value in MSG_FUNC
    uint16_t m_msgObj;      ///< value function to type and group to specify the object
    uint8_t m_payload[COM_MSG_SIZE];

    enum MSG_TYPE {
        MSG_TYPE_UNDEF = 0,
        MSG_TYPE_PARAM_GET,      ///< kind of command to request a parameter value
        MSG_TYPE_PARAM_SET,      ///< kind of command to set a parameter value. Component reply with the new value to ack.
        MSG_TYPE_PARAM_VALUE,    ///< kind of event to return a parameter value
        MSG_TYPE_CMD,
        MSG_TYPE_EVENT,
        MSG_TYPE_END_ENUM
    };

    enum DEVICE_FUNC {
        DEVICE_FUNC_UNDEF                   = 0,
        DEVICE_FUNC_GENERAL,
        DEVICE_FUNC_ASCAN_0,
        DEVICE_FUNC_ASCAN_1,
        DEVICE_FUNC_ASCAN_2,
        DEVICE_FUNC_ASCAN_3,
        DEVICE_FUNC_POSITION,
        DEVICE_FUNC_MOVE,
        DEVICE_FUNC_PUMP,
        DEVICE_FUNC_LIGHT,
        DEVICE_FUNC_INERTIAL_SENSOR         = 10,
        DEVICE_FUNC_PRESSURE_SENSOR_0,
        DEVICE_FUNC_PRESSURE_SENSOR_1,
        DEVICE_FUNC_PRESSURE_SENSOR_2,
        DEVICE_FUNC_TEMPERATURE_SENSOR_0,
        DEVICE_FUNC_TEMPERATURE_SENSOR_1,
        DEVICE_FUNC_TEMPERATURE_SENSOR_2,
        DEVICE_FUNC_HUMIDITY_SENSOR_0,
        DEVICE_FUNC_HUMIDITY_SENSOR_1,
        DEVICE_FUNC_HUMIDITY_SENSOR_2,
        DEVICE_FUNC_BATTERY                 = 20,
        DEVICE_FUNC_CLEANING,
        DEVICE_FUNC_ENDSTOP,
        DEVICE_FUNC_LICENCE,
        DEVICE_FUNC_COMPASS,
        DEVICE_FUNC_END_ENUM
    };

    enum PROD_ID {
        PROD_UNDEF          = 0,
        PROD_UT_POLE        = 1,    // ASCAN0, POSITION, PUMP, LIGHT, PRESSURE0
        PROD_ALTISCAN_V2    = 2,    // ASCAN0, POSITION, MOVE, PUMP, LIGHT, INERTIAL, PRESSURE0
        PROD_ROVER          = 3,    // ASCAN0, POSITION, MOVE, PUMP, LIGHT
        PROD_ROVER_IP68     = 4,    // ASCAN0, POSITION, MOVE, LIGHT, INERTIAL, PRESSURE0
        PROD_ROVER_CLEANER  = 5,    // POSITION, MOVE, LIGHT, INERTIAL, PRESSURE0
        PROD_ALTISCAN_V14   = 6,    // ASCAN0, POSITION, MOVE
        PROD_PANDA_VOL3     = 7,    // ASCAN0, ASCAN1, ASCAN2, LIGHT
        PROD_MULTISCAN      = 8,    // ASCAN0, ASCAN1, ASCAN2, ASCAN3, POSITION, MOVE, PUMP, LIGHT, INERTIAL
        PROD_RED_PITAYA     = 9,
        PROD_MAX_ID
    };

    static QString prodName(quint8 p_prodId);

    // --------------------------------
    // -- PROTOCOL VERSION
    //
    // The authentication request (C_GENERAL_REQ_AUTH) is identical for both protocols.
    // The protocol in use is determined by the LEN of the answer (C_GENERAL_ANS_AUTH) :
    //  - 110 bytes : legacy Datascan protocol (function list inside the answer)
    //  -  92 bytes : DatascanV4 protocol D4.x (PROTOCOL_KEY inside the answer, functions
    //                requested afterwards with C_GENERAL_REQUEST_FUNC)

    enum PROTOCOL_VERSION {
        PROTOCOL_UNKNOWN = 0,   ///< not authentified yet
        PROTOCOL_LEGACY,        ///< old Datascan protocol (ANS_AUTH LEN = 110)
        PROTOCOL_D4             ///< DatascanV4 protocol D4.x (ANS_AUTH LEN = 92)
    };

    static QString protocolName(quint8 p_protocol);

    static const int32_t AUTH_KEY_SIZE = 32;
    static const uint16_t ANS_AUTH_LEN_LEGACY = 110;
    static const uint16_t ANS_AUTH_LEN_D4 = 92;

    static const uint8_t PROTOCOL_KEY_FAMILY_D4 = 0xD4;     ///< PROTOCOL_KEY[0] : DatascanV4 protocol family
    static const uint8_t PROTOCOL_KEY_GENERATION_D4 = 0x01; ///< PROTOCOL_KEY[1] : supported compatibility generation (D4 1.x.x)

    /// Payload offsets of C_GENERAL_ANS_AUTH in protocol D4
    enum ANS_AUTH_D4_OFFSET {
        ANS_AUTH_D4_OFS_KEY         = 0,
        ANS_AUTH_D4_OFS_PROD_ID     = 32,
        ANS_AUTH_D4_OFS_DEVICE_AD   = 33,
        ANS_AUTH_D4_OFS_PROD_UID    = 34,
        ANS_AUTH_D4_OFS_VER_HW      = 50,
        ANS_AUTH_D4_OFS_VER_SW      = 62,
        ANS_AUTH_D4_OFS_PROTO_KEY   = 74,
        ANS_AUTH_D4_OFS_DATE_BUILD  = 76,
        ANS_AUTH_D4_OFS_DATE_REV    = 84
    };
    static const int32_t ANS_AUTH_D4_PROD_UID_SIZE = 16;
    static const int32_t ANS_AUTH_D4_VER_SIZE = 12;

    enum MSG_OBJ {
        MSG_OBJ_UNDEF = 0
    };

    // --------------------------------
    // -- FUNCTION GENERAL

    enum FUNC_GENERAL_PARAM {
        FUNC_GENERAL_PARAM_UNDEF = 0,
        FUNC_GENERAL_PARAM_D_PROD_ID,               ///< PAYLOAD : PRODUCT ID (uint8_t : 1 byte)
        FUNC_GENERAL_PARAM_D_DEVICE_AD,             ///< PAYLOAD : DEVICE ADDRESS (uint8_t : 1 byte)
        FUNC_GENERAL_PARAM_D_PROD_UID,              ///< PAYLOAD : 16 bytes with Product UID (serial number)
        FUNC_GENERAL_PARAM_D_VER_HW,                ///< PAYLOAD : 12 bytes with the device hardware version (example : "123.456\0")
        FUNC_GENERAL_PARAM_D_VER_SW,                ///< PAYLOAD : 12 bytes with the device software version (example : "186.547-DEV\0")
        FUNC_GENERAL_PARAM_D_DATE_BUILD,            ///< PAYLOAD : 8 bytes with the build date. Epoch time in seconds from 1970
        FUNC_GENERAL_PARAM_D_DATE_REV,              ///< PAYLOAD : 8 bytes with the last revision date. Epoch time in seconds from 1970
        FUNC_GENERAL_PARAM_D_FCTS                   ///< PAYLOAD : array of 20 bytes : 20 functions max. 0 = no functions
    };

    enum FUNC_GENERAL_EVENT {
        FUNC_GENERAL_EVENT_UNDEF = 0,        
        FUNC_GENERAL_EVENT_E_HEART_BEAT,            ///< PAYLOAD : EMPTY
        FUNC_GENERAL_EVENT_E_LOG,                   ///< PAYLOAD : LEVEL (1 byte) + STRING(PAYLOAD len - 1)
        FUNC_GENERAL_EVENT_E_ERROR,                 ///< PAYLOAD : CODE ERROR (int32 (4 bytes)) + STRING(PAYLOAD len - 4)
        FUNC_GENERAL_EVENT_E_ALERT_MESSAGE          ///< PAYLOAD : ALERT LEVEL (uint8_t 1 byte) + STRING(PAYLOAD len - 1)
    };

    enum FUNC_GENERAL_CMD {
        FUNC_GENERAL_CMD_UNDEF = 0,
        FUNC_GENERAL_CMD_C_REQ_AUTH,                ///< PAYLOAD : 32 bytes (crypted data generated by the MPC to the device)
        FUNC_GENERAL_CMD_C_ANS_AUTH,                ///< PAYLOAD : 100 bytes : crypted data [32 bytes] + Product ID [1 byte] + Device AD [1 byte] + Function List [20 bytes] + Product UID [16 bytes] + VER_HW [12 bytes] + VER_SW [12 bytes] + DATE_BUILD [8 bytes] + DATE_REV [8 bytes]
        FUNC_GENERAL_CMD_C_EMERGENCY_STOP,          ///< PAYLOAD : EMPTY
        FUNC_GENERAL_CMD_C_REQUEST_FUNC,            ///< PAYLOAD : EMPTY
        FUNC_GENERAL_CMD_C_ANS_FUNC                 ///< PAYLOAD : N + N couples [functions][requests]
    };

    // --------------------------------
    // -- FUNCTION ASCAN

    enum FUNC_ASCAN_PARAM {
        FUNC_ASCAN_PARAM_D_UNDEF = 0,
        FUNC_ASCAN_PARAM_D_GAIN,            ///< PAYLOAD : Gain of the US-SPI (uint8_t : 1 byte [0-80])
        FUNC_ASCAN_PARAM_D_VOLTAGE,         ///< PAYLOAD : Pulse voltage adjustment of the US-SPI (uint8_t : 1 byte [10-230])
        FUNC_ASCAN_PARAM_D_UT_ELEMENT,      ///< PAYLOAD : Number of elements on UT probe (uint8_t : 1 byte : 1 for mono-element, or 2 for bi-element
        FUNC_ASCAN_PARAM_D_SAMPLE_FREQ,     ///< PAYLOAD : Sample frequency of UT (uint8_t : 1 byte : 0 = 80 MHz, 1 = 40 MHz, 2 = 20 MHz)
        FUNC_ASCAN_PARAM_D_PULSE_WIDTH,     ///< PAYLOAD : Pulse width in ns, adjustment of the US-SPI (uint16_t : 2 bytes [18-1600]).

        FUNC_ASCAN_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_ASCAN_PARAM_L_CELERITY,         ///< Celerity : speed of US in the material
        FUNC_ASCAN_PARAM_L_PROBE_DELAY,
        FUNC_ASCAN_PARAM_L_LOWPASS_FILTER,
        FUNC_ASCAN_PARAM_L_GATE0_START,
        FUNC_ASCAN_PARAM_L_GATE0_WIDTH,
        FUNC_ASCAN_PARAM_L_GATE0_HEIGHT,
        FUNC_ASCAN_PARAM_L_GATE1_START,
        FUNC_ASCAN_PARAM_L_GATE1_WIDTH,
        FUNC_ASCAN_PARAM_L_GATE1_HEIGHT
    };

    enum FUNC_ASCAN_EVENT {
        FUNC_ASCAN_EVENT_UNDEF = 0,
        FUNC_ASCAN_EVENT_D_SAMPLE,          ///< PAYLOAD : 4000 samples (1 sample = 1 byte)
        FUNC_ASCAN_EVENT_D_SAMPLE_PART_1_4, ///< PAYLOAD : 1000 first samples of one ASCAN => initiate the new ASCAN
        FUNC_ASCAN_EVENT_D_SAMPLE_PART_2_4, ///< PAYLOAD : 1000 samples
        FUNC_ASCAN_EVENT_D_SAMPLE_PART_3_4, ///< PAYLOAD : 1000 samples
        FUNC_ASCAN_EVENT_D_SAMPLE_PART_4_4  ///< PAYLOAD : 1000 last samples of one ASCAN => emit new msg
    };

    enum FUNC_ASCAN_CMD {
        FUNC_ASCAN_CMD_C_UNDEF = 0,
        FUNC_ASCAN_CMD_C_ENABLE         ///< PAYLOAD : 1 Bytes (0 = stop, 1 = start)
    };

    // --------------------------------
    // -- FUNCTION POSITION

    enum FUNC_POSITION_PARAM {
        FUNC_POSITION_PARAM_D_UNDEF = 0,
        FUNC_POSITION_PARAM_D_ODO_POS_MM_X,          ///< PAYLOAD : Position of the vector in mm (qint32 : 4 bytes)
        FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X,
        FUNC_POSITION_PARAM_D_ODO_RES_X,
        FUNC_POSITION_PARAM_D_ODO_FACTOR_X,
        FUNC_POSITION_PARAM_D_WHEEL_SPACING_MM,         ///< 2 bytes (uint16) : écartement entre les 2 roues motrices du porteur en mm
        FUNC_POSITION_PARAM_D_MIN_TURNING_RADIUS_MM,    ///< 2 bytes (uint16) : rayon de braquage minimum du porteur en mm. 0 : autorise la rotation sur place du porteur

        FUNC_POSITION_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X,
        FUNC_POSITION_PARAM_L_VECTOR_DIR_X
    };

    // --------------------------------
    // -- FUNCTION MOVE

    enum FUNC_MOVE_CMD {
        FUNC_MOVE_CMD_C_UNDEF = 0,
        FUNC_MOVE_CMD_C_REQ_JOY_X_Y,                            ///< Payload : 2 bytes : [X][Y], 127 = neutre, 0 = débattement min, 254 = débattement max
        FUNC_MOVE_CMD_C_ANS_JOY_X_Y,                            ///< Payload : 2 bytes : [X][Y], 127 = neutre, 0 = débattement min, 254 = débattement max
        FUNC_MOVE_CMD_C_REQ_JOY_X_Y_CLEAN_BRUSH,                ///< Payload : 4 bytes : [X][Y], 127 = neutre, 0 = débattement min, 254 = débattement max
        FUNC_MOVE_CMD_C_REQ_JOY_X_Y_ANGLE_CONTROL               ///< Payload : 4 bytes : [X][Y], 127 = neutre, 0 = débattement min, 254 = débattement max
    };

    // --------------------------------
    // -- FUNCTION PUMP

    enum FUNC_PUMP_PARAM {
        FUNC_PUMP_PARAM_D_UNDEF = 0,
        FUNC_PUMP_PARAM_D_PUMP,                                 ///< 0 to 100%, set device pump level
        FUNC_PUMP_PARAM_D_TANK_LVL,

        FUNC_PUMP_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_PUMP_PARAM_L_PUMP_ENABLE,                          ///< 0/1
        FUNC_PUMP_PARAM_L_PUMP_LEVEL                            ///< 0 to 100%, send to pump if L_PUMP_ENABLE = 1
    };

    // --------------------------------
    // -- FUNCTION LIGHT

    enum FUNC_LIGHT_PARAM {
        FUNC_LIGHT_PARAM_D_UNDEF = 0,
        FUNC_LIGHT_PARAM_D_LIGHT,                               ///< 0 to 100%, set device LIGHT level

        FUNC_LIGHT_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_LIGHT_PARAM_L_LIGHT_ENABLE,                        ///< 0/1
        FUNC_LIGHT_PARAM_L_LIGHT_LEVEL                          ///< 0 to 100%, send to LIGHT if L_LIGHT_ENABLE = 1
    };

    // --------------------------------
    // -- FUNCTION INERTIAL SENSOR

    enum FUNC_INERTIAL_SENSOR_PARAM {
        FUNC_INERTIAL_SENSOR_PARAM_D_UNDEF = 0,
        FUNC_INERTIAL_SENSOR_PARAM_D_ACC_X_Y_Z,                 ///< X,Y et Z : valeur 16 bits signés, en Gx1000 (1.002G = 1002). Payload 6 bytes : [X_LSB][X_MSB][Y_LSB][Y_MSB][Z_LSB][Z_MSB]

        FUNC_INERTIAL_SENSOR_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_INERTIAL_SENSOR_PARAM_L_DIRECTION,
        FUNC_INERTIAL_SENSOR_PARAM_L_HORIZONTAL
    };

    // --------------------------------
    // -- FUNCTION PRESSURE SENSOR

    enum FUNC_PRESSURE_SENSOR_PARAM {
        FUNC_PRESSURE_SENSOR_PARAM_D_UNDEF = 0,
        FUNC_PRESSURE_SENSOR_PARAM_D_PRESSURE_B,                ///< Valeur de la pression 16 bits non signés : 1 Bar = 1000 (exemple : 1.008Bar = 1008)

        FUNC_PRESSURE_SENSOR_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_PRESSURE_SENSOR_PARAM_L_DEPTH_VALUE_M,             ///< Valeur de la profondeur en mètre. Payload : 8 bytes  (double)
        FUNC_PRESSURE_SENSOR_PARAM_L_PRESSURE_OFFSET            ///< Valeur de l’offset de la pression (nombre de bars à 0 m de profondeur). Payload : 2 bytes
    };

    // --------------------------------
    // -- FUNCTION TEMPERATURE SENSOR

    enum FUNC_TEMPERATURE_SENSOR_PARAM {
        FUNC_TEMPERATURE_SENSOR_PARAM_D_UNDEF = 0,
        FUNC_TEMPERATURE_SENSOR_PARAM_D_TEMPERATURE_C           ///< Valeur de la température 16 bits signés : 1 degré = 10 (exemple : 165 = 16.5 degrés). Payload : 2 bytes
    };

    // --------------------------------
    // -- FUNCTION HUMIDITY SENSOR

    enum FUNC_HUMIDITY_SENSOR_PARAM {
        FUNC_HUMIDITY_SENSOR_PARAM_D_UNDEF = 0,
        FUNC_HUMIDITY_SENSOR_PARAM_D_HUMIDITY_P                 ///< Valeur du pourcentage d’humidité 16 bits non signés : 1 pour-cent = 10 (942 = 94.2%). Payload : 2 bytes
    };

    // --------------------------------
    // -- FUNCTION BATTERY SENSOR

    enum FUNC_BATTERY_PARAM {
        FUNC_BATTERY_PARAM_D_UNDEF = 0,
        FUNC_BATTERY_PARAM_D_BATTERY_LVL_P                      ///< PAYLOAD : 1 byte : percent of battery : 0 - 100%. 255 = no battery
    };

    // --------------------------------
    // -- FUNCTION CLEANING

    enum FUNC_CLEANING_PARAM {
        FUNC_CLEANING_PARAM_D_UNDEF = 0,
        FUNC_CLEANING_PARAM_D_CLEANING_BRUSH,
        FUNC_CLEANING_PARAM_D_CLEANING_PUMP,
        FUNC_CLEANING_PARAM_D_TANK_LVL,

        FUNC_CLEANING_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_ENABLE,
        FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_LEVEL,
        FUNC_CLEANING_PARAM_L_CLEANING_PUMP_ENABLE,
        FUNC_CLEANING_PARAM_L_CLEANING_PUMP_LEVEL
    };

    // --------------------------------
    // -- FUNCTION ENDSTOP

    enum FUNC_ENDSTOP_EVENT {
        FUNC_ENDSTOP_EVENT_D_UNDEF = 0,
        FUNC_ENDSTOP_EVENT_E_ENDSTOP
    };

    enum FUNC_ENDSTOP_PARAM {
        FUNC_ENDSTOP_PARAM_D_UNDEF = 0,
        FUNC_ENDSTOP_PARAM_L_UNDEF = MSG_OBJ_PARAM_LOGICAL_FLAG,
        FUNC_ENDSTOP_PARAM_L_COLLISION
    };

    // --------------------------------
    // -- FUNCTION LICENCE

    enum FUNC_LICENCE_PARAM {
        FUNC_LICENCE_PARAM_D_UNDEF = 0,
        FUNC_LICENCE_PARAM_D_LICENCE_KEY_EQUIPMENT,
        FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P
    };

    enum FUNC_LICENCE_CMD {
        FUNC_LICENCE_CMD_D_UNDEF = 0,
        FUNC_LICENCE_CMD_D_NEW_LICENCE = 3
    };

    // --------------------------------
    // -- FUNCTION COMPASS

    enum FUNC_COMPASS_PARAM {
        FUNC_COMPASS_PARAM_D_UNDEF = 0,
        FUNC_COMPASS_PARAM_D_COMPASS_DEGR
    };

private :
    static const QString PROD_NAME[PROD_MAX_ID];

} ProtMsg;
