#include "MetaTypes.h"

void registerMetaTypes()
{
    qRegisterMetaType<ComLinkSerial*>();
    qRegisterMetaType<ComLinkTcpCli*>();
    qRegisterMetaType<ComLinkUdpCli*>();
    qRegisterMetaType<ProtMsg*>();
    qRegisterMetaType<ComLinkManager*>();
    qRegisterMetaType<ComDeviceManager*>();
    qRegisterMetaType<ComDevice*>();
    qRegisterMetaType<GenComLink*>();
    qRegisterMetaType<AScan*>();
    qRegisterMetaType<VectorFuncAScan*>();
}
