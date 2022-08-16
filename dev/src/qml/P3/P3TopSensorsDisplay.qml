import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import QtGraphicalEffects 1.0
import "../rp_components"

Rectangle {
    color: "#FFFFFF"
    border.width: 1

        GridLayout {
            id: gridLayout
            anchors.fill: parent
            anchors.margins: 10
            columns: 5
            Layout.fillWidth: true
            columnSpacing: 0

            Text {
                text: qsTr("Température (°C) : ")
                Layout.row: 1
                visible: currentVector.funcTemperatureSensor0().isEnabled
                font.pointSize: 12
            }

            Rectangle {
                visible: currentVector.funcTemperatureSensor0().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idTemperatureLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90
            }

            Rectangle {
                id: idTemperatureColorIndicator
                visible: currentVector.funcTemperatureSensor0().isEnabled
                color: "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Text {
                text: qsTr("Humidité (%) : ")
                Layout.row: 2
                visible: currentVector.funcHumiditySensor0().isEnabled
                font.pointSize: 12
            }

            Rectangle {
                visible: currentVector.funcHumiditySensor0().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idHumidityLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90

            }

            Rectangle {
                id: idHumidityColorIndicator
                visible: currentVector.funcHumiditySensor0().isEnabled
                color: "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Text {
                text: qsTr("Reserve d'eau scan (%) : ")
                Layout.row: 3
                visible: currentVector.funcPump().isEnabled
                font.pointSize: 12
            }

            Rectangle {
                visible: currentVector.funcPump().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idPumpLabel
                    text: "" //currentVector.funcPump().paramDTankLevel
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90

            }

            Rectangle {
                id: idPumpColorIndicator
                visible: currentVector.funcPump().isEnabled
                color: "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Button {
                visible: currentVector.funcPump().isEnabled
                text: "↑"
                font.pointSize: 14
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                onClicked: {
                    currentVector.funcPump().paramDTankLevel = 100
                }
            }

            Text {
                text: qsTr("Reserve d'eau brossage (%) : ")
                Layout.row: 4
                visible: currentVector.funcCleaning().isEnabled
                font.pointSize: 12
            }

            Rectangle {
                visible: currentVector.funcCleaning().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idCleaningLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90

            }

            Rectangle {
                id: idCleaningColorIndicator
                visible: currentVector.funcCleaning().isEnabled
                color: "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Button {
                visible: currentVector.funcCleaning().isEnabled
                text: "↑"
                font.pointSize: 14
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                onClicked: {
                    currentVector.funcCleaning().paramDTankLevel = 100
                }
            }

            Text {
                text: qsTr("Pression (bar) : ")
                visible: currentVector.funcPressureSensor0().isEnabled
                font.pointSize: 12
                Layout.row: 5
            }

            Rectangle {
                visible: currentVector.funcPressureSensor0().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idPressureLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90
            }

            Text {
                text: qsTr("↳ Hauteur (m) : ")
                visible: currentVector.funcPressureSensor0().isEnabled
                color: "#555555"
                font.pointSize: 12
                Layout.row: 6
            }

            Rectangle {
                visible: currentVector.funcPressureSensor0().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idDepthLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90
            }

            Button {
                visible: currentVector.funcPressureSensor0().isEnabled
                text: qsTr("Tare")
                font.pointSize: 12
                Layout.preferredHeight: 65
                Layout.preferredWidth: 60
                Layout.rowSpan: 2
                Layout.columnSpan: 2
                Layout.row: 5
                Layout.column: 2
                onClicked: {
                    currentVector.funcPressureSensor0().calibrateDepth0();
                }
            }

            Text {
                text: qsTr("Boussole (degr) : ")
                Layout.row: 7
                visible: currentVector.funcCompass().isEnabled
                font.pointSize: 12
            }

            Rectangle {
                visible: currentVector.funcCompass().isEnabled
                border.color: "#E5E5E5"
                border.width: 1

                Text {
                    id: idCompassLabel
                    text: ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                MouseArea {
                    id: idCompassTooltipArea
                    anchors.fill: parent
                    hoverEnabled: true
                }

                ToolTip.delay: 500
                ToolTip.timeout: 5000
                ToolTip.visible: idCompassTooltipArea.containsMouse
                ToolTip.text: qsTr("Nord = 0 degrés ; Est = 90 degrés")

                height: 30
                width: 90
            }

            Connections {
                target: currentVector.funcTemperatureSensor0()
                function onEvUpdateTemperature(raw_temperature) {
                    var temperature = (raw_temperature / 10.0).toFixed(2)
                    idTemperatureColorIndicator.color = (temperature < 75) ? "#E5E5E5" : "red"
                    idTemperatureLabel.text = temperature + " °C"
                }
            }

            Connections {
                target: currentVector.funcHumiditySensor0()
                function onEvUpdateHumidity(raw_humidity) {
                    var humidity = (raw_humidity / 10.0).toFixed(2)
                    idHumidityColorIndicator.color = (humidity < 65) ? "#E5E5E5" : "red"
                    idHumidityLabel.text = humidity + " %"
                }
            }

            Connections {
                target: currentVector.funcPump()
                function onEvUpdateTankLevel(tank_level) {
                    idPumpLabel.text = tank_level
                    idPumpColorIndicator.color = (tank_level > 15) ? "blue" : "#A0A0FF"
                }
            }

            Connections {
                target: currentVector.funcCleaning()
                function onEvUpdateTankLevel(tank_level) {
                    idCleaningLabel.text = tank_level
                    idCleaningColorIndicator.color =(tank_level > 15) ? "blue" : "#A0A0FF"
                }
            }

            Connections {
                target: currentVector.funcPressureSensor0()
                function onEvUpdatePressure(pressure) {
                    idPressureLabel.text = (pressure / 1000.0) + " bar"
                    idDepthLabel.text = currentVector.funcPressureSensor0().depthStrValue
                }
            }

            Connections {
                target: currentVector.funcPressureSensor0()
                function onDepthStrValueChanged(depth) {
                    idDepthLabel.text = depth
                }
            }

            Connections {
                target: currentVector.funcCompass()
                function onEvUpdateCompass(orientation) {
                    idCompassLabel.text = orientation
                }
            }

            Item {
                Layout.row: 11
                Layout.column: 1
                Layout.fillHeight: true
            }

            Item {
                Layout.row: 10
                Layout.column: 4
                Layout.fillWidth: true
            }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
