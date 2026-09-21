#pragma once

#include <QMetaType>

// Forward declarations
class ComLinkSerial;
class ComLinkTcpCli;
class ComLinkUdpCli;
class ProtMsg;
class ComLinkManager;
class ComDeviceManager;
class ComDevice;
class GenComLink;
class AScan;

// OPAQUE POINTERS
Q_DECLARE_OPAQUE_POINTER(ComLinkSerial*)
Q_DECLARE_OPAQUE_POINTER(ComLinkTcpCli*)
Q_DECLARE_OPAQUE_POINTER(ComLinkUdpCli*)
Q_DECLARE_OPAQUE_POINTER(ProtMsg*)
Q_DECLARE_OPAQUE_POINTER(ComLinkManager*)
Q_DECLARE_OPAQUE_POINTER(ComDeviceManager*)
Q_DECLARE_OPAQUE_POINTER(ComDevice*)
Q_DECLARE_OPAQUE_POINTER(GenComLink*)
Q_DECLARE_OPAQUE_POINTER(AScan*)

// METATYPES
Q_DECLARE_METATYPE(ComLinkSerial*)
Q_DECLARE_METATYPE(ComLinkTcpCli*)
Q_DECLARE_METATYPE(ComLinkUdpCli*)
Q_DECLARE_METATYPE(ProtMsg*)
Q_DECLARE_METATYPE(ComLinkManager*)
Q_DECLARE_METATYPE(ComDeviceManager*)
Q_DECLARE_METATYPE(ComDevice*)
Q_DECLARE_METATYPE(GenComLink*)
Q_DECLARE_METATYPE(AScan*)
