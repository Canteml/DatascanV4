#include <v2/prot/ProtD4.h>
#include <v2/prot/ProtD4Key.h>

#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <cstring>

namespace {

struct InstanceFunc { quint8 wire; quint8 count; };
const InstanceFunc INSTANCE_FUNCS[] = { {0x02, 4}, {0x0B, 3}, {0x0E, 3}, {0x11, 3} };

bool isReserved(quint8 p_func)
{
    return (p_func >= 0x03 && p_func <= 0x05) || p_func == 0x0C || p_func == 0x0D
           || p_func == 0x0F || p_func == 0x10 || p_func == 0x12 || p_func == 0x13;
}

const InstanceFunc *findByWire(quint8 p_func)
{
    for( const InstanceFunc &f : INSTANCE_FUNCS ) {
        if( f.wire == p_func ) return &f;
    }
    return nullptr;
}

const InstanceFunc *findByInternal(quint8 p_func)
{
    for( const InstanceFunc &f : INSTANCE_FUNCS ) {
        if( p_func >= f.wire && p_func < f.wire + f.count ) return &f;
    }
    return nullptr;
}

int expectedParamLen(quint8 p_func, quint8 p_obj)
{
    switch( p_func ) {
    case ProtMsg::DEVICE_FUNC_POSITION:
        switch( p_obj ) { case 1: case 4: return 4; case 2: case 3: case 5: case 6: return 2; default: return -1; }
    case ProtMsg::DEVICE_FUNC_PUMP:   return (p_obj == 1 || p_obj == 2) ? 1 : -1;
    case ProtMsg::DEVICE_FUNC_LIGHT:  return p_obj == 1 ? 1 : -1;
    case ProtMsg::DEVICE_FUNC_INERTIAL_SENSOR: return p_obj == 1 ? 6 : -1;
    case 0x0B: case 0x0E: case 0x11:  return p_obj == 1 ? 2 : -1;
    case ProtMsg::DEVICE_FUNC_BATTERY:  return p_obj == 1 ? 1 : -1;
    case ProtMsg::DEVICE_FUNC_CLEANING: return p_obj == 1 ? 1 : -1;
    case ProtMsg::DEVICE_FUNC_LICENCE:  return p_obj == 1 ? 135 : (p_obj == 2 ? 1 : -1);
    case ProtMsg::DEVICE_FUNC_COMPASS:  return p_obj == 1 ? 2 : -1;
    default: return -1;
    }
}

void padString(ProtMsg *p_msg, int p_size)
{
    const int used = qMin<int>(p_msg->m_len, p_size - 1);
    std::memset(p_msg->m_payload + used, 0, size_t(p_size - used));
    p_msg->m_len = quint16(p_size);
}

bool adaptGeneralToWire(ProtMsg *p_msg)
{
    if( ProtMsg::MSG_TYPE_PARAM_SET != p_msg->m_msgType ) return true;
    switch( p_msg->m_msgObj ) {
    case ProtMsg::FUNC_GENERAL_PARAM_D_PROD_ID:
    case ProtMsg::FUNC_GENERAL_PARAM_D_DEVICE_AD: p_msg->m_len = 1; return true;
    case ProtMsg::FUNC_GENERAL_PARAM_D_PROD_UID:  padString(p_msg, 16); return true;
    case ProtMsg::FUNC_GENERAL_PARAM_D_VER_HW:
    case ProtMsg::FUNC_GENERAL_PARAM_D_VER_SW:    padString(p_msg, 12); return true;
    case ProtMsg::FUNC_GENERAL_PARAM_D_FCTS:      return false;
    default: return true;
    }
}

void adaptPositionToWire(ProtMsg *p_msg)
{
    if( ProtMsg::MSG_TYPE_PARAM_SET != p_msg->m_msgType ) return;
    if( ProtMsg::FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X == p_msg->m_msgObj ) {
        quint16 mm;
        std::memcpy(&mm, p_msg->m_payload, 2);
        const quint16 x10 = quint16(qMin<quint32>(quint32(mm) * 10u, 65535u));
        std::memcpy(p_msg->m_payload, &x10, 2);
    } else if( ProtMsg::FUNC_POSITION_PARAM_D_ODO_FACTOR_X == p_msg->m_msgObj ) {
        qreal factor;
        std::memcpy(&factor, p_msg->m_payload, 8);
        const quint32 value = factor > 0 ? quint32(qRound64(factor)) : 0u;
        std::memcpy(p_msg->m_payload, &value, 4);
        p_msg->m_len = 4;
    }
}

void adaptPositionFromWire(ProtMsg *p_msg)
{
    if( ProtMsg::FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X == p_msg->m_msgObj ) {
        quint16 x10;
        std::memcpy(&x10, p_msg->m_payload, 2);
        const quint16 mm = quint16((quint32(x10) + 5u) / 10u);
        std::memcpy(p_msg->m_payload, &mm, 2);
    } else if( ProtMsg::FUNC_POSITION_PARAM_D_ODO_FACTOR_X == p_msg->m_msgObj ) {
        quint32 value;
        std::memcpy(&value, p_msg->m_payload, 4);
        const qreal factor = value;
        std::memcpy(p_msg->m_payload, &factor, 8);
        p_msg->m_len = 8;
    }
}

}

namespace ProtD4 {

bool verifyAuthAnswer(const qint8 *p_request, const quint8 *p_answer)
{
    const QByteArray key = QByteArray::fromHex(D4_HMAC_KEY_HEX);
    if( key.size() != ProtMsg::AUTH_KEY_SIZE ) return false;
    const QByteArray request(reinterpret_cast<const char *>(p_request), ProtMsg::AUTH_KEY_SIZE);
    const QByteArray expected = QMessageAuthenticationCode::hash(request, key, QCryptographicHash::Sha256);
    quint8 diff = 0;
    for( int i = 0; i < ProtMsg::AUTH_KEY_SIZE; i++ ) {
        diff |= quint8(expected.at(i)) ^ p_answer[i];
    }
    return 0 == diff;
}

QList<quint32> decodeFunctionList(const quint8 *p_payload, int p_len, bool *p_ok)
{
    QList<quint32> list;
    *p_ok = p_len >= 1 && p_len == 2 * int(p_payload[0]) + 1;
    if( !*p_ok ) return list;

    for( int i = 0; i < p_payload[0]; i++ ) {
        const quint8 func = p_payload[1 + 2 * i];
        const quint8 count = p_payload[2 + 2 * i];
        if( 0 == count || ProtMsg::DEVICE_FUNC_GENERAL == func || isReserved(func)
            || func >= ProtMsg::DEVICE_FUNC_END_ENUM ) {
            continue;
        }
        const InstanceFunc *multi = findByWire(func);
        const int instances = multi ? qMin<int>(count, multi->count) : 1;
        for( int k = 0; k < instances; k++ ) {
            if( !list.contains(quint32(func + k)) ) list.append(quint32(func + k));
        }
    }
    return list;
}

bool toWire(ProtMsg *p_msg)
{
    const quint8 obj = quint8(p_msg->m_msgObj & 0xFF);
    if( obj >= ProtMsg::MSG_OBJ_PARAM_LOGICAL_FLAG ) return false;

    quint8 wireFunc = p_msg->m_deviceFunc;
    quint8 instance = 0;
    if( const InstanceFunc *multi = findByInternal(p_msg->m_deviceFunc) ) {
        wireFunc = multi->wire;
        instance = quint8(p_msg->m_deviceFunc - multi->wire);
    }

    switch( wireFunc ) {
    case ProtMsg::DEVICE_FUNC_GENERAL:
        if( !adaptGeneralToWire(p_msg) ) return false;
        break;
    case ProtMsg::DEVICE_FUNC_ASCAN_0:
        if( ProtMsg::MSG_TYPE_CMD != p_msg->m_msgType ) return false;
        break;
    case ProtMsg::DEVICE_FUNC_POSITION:
        adaptPositionToWire(p_msg);
        break;
    case ProtMsg::DEVICE_FUNC_MOVE:
        if(ProtMsg::MSG_TYPE_CMD != p_msg->m_msgType ) return false;
        if( ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y != obj && ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y_ANGLE_CONTROL != obj)
            return false;
        // Intern: [0] = rotation, [1] = forward/backward, D4 : [0] = forward/backward, [1] = rotation
        qSwap(p_msg->m_payload[0], p_msg->m_payload[1]);
        p_msg->m_len = 2;
        p_msg->m_msgObj = ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y;
        return true;
    default:
        break;
    }

    p_msg->m_deviceFunc = wireFunc;
    p_msg->m_msgObj = quint16(obj) | (quint16(instance) << 8);
    return true;
}

bool fromWire(ProtMsg *p_msg)
{
    const quint8 obj = quint8(p_msg->m_msgObj & 0xFF);
    const quint8 instance = quint8(p_msg->m_msgObj >> 8);
    const quint8 wireFunc = p_msg->m_deviceFunc;

    if( wireFunc >= ProtMsg::DEVICE_FUNC_END_ENUM || isReserved(wireFunc) ) return false;

    quint8 internalFunc = wireFunc;
    if( const InstanceFunc *multi = findByWire(wireFunc) ) {
        if( instance >= multi->count ) return false;
        internalFunc = quint8(wireFunc + instance);
    } else if( 0 != instance ) {
        return false;
    }

    if( ProtMsg::MSG_TYPE_PARAM_VALUE == p_msg->m_msgType ) {
        const int expected = expectedParamLen(wireFunc, obj);
        if( expected >= 0 && expected != p_msg->m_len ) return false;
    }

    p_msg->m_deviceFunc = internalFunc;
    p_msg->m_msgObj = obj;
    if( ProtMsg::DEVICE_FUNC_POSITION == wireFunc && ProtMsg::MSG_TYPE_PARAM_VALUE == p_msg->m_msgType ) {
        adaptPositionFromWire(p_msg);
    }
    return true;
}

}