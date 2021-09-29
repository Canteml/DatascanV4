import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import "../rp_components"

Rectangle {

    color: "#FFFFFF"
    border.width: 1

    // -- SETTINGS SENSOR
    GridLayout {
        id: gridLayout
        columns: 2
        anchors.fill: parent

        anchors.margins: 10

        // --
        Text {
            text: qsTr("Echelle X (m) :")
            font.pointSize: 12
        }

        RpSpinBox {
            id: idScaleX
            from: 1
            to: 10
            stepSize: 1
            editable: true
            font.pointSize: 12

            value: scanManager.BScanAxeXScaleM

            onValueChanged: {
                scanManager.BScanAxeXScaleM = value
            }
            onValueModified: {
                focus = false;
            }
        }

        //--
        Text {
            text: qsTr("Echelle Y min (mm) :")
            font.pointSize: 12
        }

        RpSpinBox {
            id: idScaleYMin
            from: 0
            to: idScaleYMax.value-1
            stepSize: 1
            editable: true
            font.pointSize: 12

            value: scanManager.BScanAxeYMinMm

            onValueChanged: {
                scanManager.BScanAxeYMinMm = value
            }

            onValueModified: {
                focus = false;
            }
        }

        //--
        Text {
            text: qsTr("Echelle Y max (mm) :")
            font.pointSize: 12
        }

        RpSpinBox {
            id: idScaleYMax
            from: idScaleYMin.value + 1
            to: 200
            stepSize: 1
            editable: true
            font.pointSize: 12

            value: scanManager.BScanAxeYMaxMm

            onValueChanged: {
                scanManager.BScanAxeYMaxMm = value
            }

            onValueModified: {
                focus = false;
            }
        }

        //--
        Text {
            text: qsTr("Zone auto détection (mm) :")
            font.pointSize: 12
        }

        RpSpinBox {
            id: idAutoZoneHole
            from: 10
            to: 100
            stepSize: 5
            editable: true
            font.pointSize: 12

            value: examManager.zoneAutoHoleSizeMm

            onValueChanged: {
                examManager.zoneAutoHoleSizeMm = idAutoZoneHole.value
            }

            onValueModified: {
                focus = false;
            }
        }
        // spacer
        Item {
            Layout.fillHeight: true
        }

        // spacer
        Item {
            Layout.fillWidth: true
        }
    }
}
