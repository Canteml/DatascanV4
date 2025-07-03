/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#include <ComLinkManager.h>
#include <ComLinkModel.h>
#include <ComLinkSerial.h>
#include <ComLinkTcpConfig.h>
#include <ComLinkUdpCli.h>
#include <ComLinkUdpConfig.h>
#include <GenComLink.h>
#include <IntComMsg.h>
#include <MepTableModel.h>
#include <VectorFunc.h>
#include <VectorFuncAScan.h>
#include <VectorFuncBattery.h>
#include <VectorFuncCleaning.h>
#include <VectorFuncCompass.h>
#include <VectorFuncEndStop.h>
#include <VectorFuncHumiditySensor.h>
#include <VectorFuncInertialSensor.h>
#include <VectorFuncLicence.h>
#include <VectorFuncLight.h>
#include <VectorFuncMove.h>
#include <VectorFuncPosition.h>
#include <VectorFuncPressureSensor.h>
#include <VectorFuncPump.h>
#include <VectorFuncTemperatureSensor.h>
#include <VectorManager.h>
#include <VectorStd.h>

void qml_register_types_roboplanet_v2()
{
    qmlRegisterTypesAndRevisions<ComLinkManager>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkModel>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkSerial>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkSerialConfig>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkTcpConfig>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkUdpCli>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<ComLinkUdpConfig>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<GenComLink>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<GenVectorFunc>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<IntComMsg>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<MepTableModel>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<MepVirolesHeaderModel>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncAScan>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncBattery>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncCleaning>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncCompass>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncEndStop>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncHumiditySensor>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncInertialSensor>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncLicence>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncLight>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncMove>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncPosition>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncPressureSensor>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncPump>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorFuncTemperatureSensor>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorManager>("roboplanet.v2", 1);
    qmlRegisterTypesAndRevisions<VectorStd>("roboplanet.v2", 1);
    qmlRegisterModule("roboplanet.v2", 1, 0);
}

static const QQmlModuleRegistration registration("roboplanet.v2", 1, qml_register_types_roboplanet_v2);
