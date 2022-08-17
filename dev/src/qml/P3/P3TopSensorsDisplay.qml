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
                    text: currentVector.funcTemperatureSensor0().hasReceivedMessage ?
                              (currentVector.funcTemperatureSensor0().paramDTemperatureC / 10.0).toFixed(2) : ""
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
                color: currentVector.funcTemperatureSensor0().hasReceivedMessage ?
                           currentVector.funcTemperatureSensor0().paramDTemperatureC >= 750 ?
                               "red" : "#E5E5E5" : "#E5E5E5"
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
                    text: currentVector.funcHumiditySensor0().hasReceivedMessage ?
                              (currentVector.funcHumiditySensor0().paramDHumidityC / 10.0).toFixed(2) : ""
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
                color: currentVector.funcHumiditySensor0().hasReceivedMessage ?
                           currentVector.funcHumiditySensor0().paramDHumidityC >= 650 ?
                               "red" : "#E5E5E5" : "#E5E5E5"
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
                    text: currentVector.funcPump().hasReceivedMessage ?
                              currentVector.funcPump().paramDTankLevel : ""
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
                color: currentVector.funcPump().hasReceivedMessage ?
                           currentVector.funcPump().paramDTankLevel > 15 ?
                               "blue" : "#A0A0FF" : "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Button {
                id: idPumpTankButton
                visible: currentVector.funcPump().isEnabled
                enabled: currentVector.funcPump().hasReceivedMessage
                text: "↑"
                font.pointSize: 14
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                onClicked: {
                    currentVector.funcPump().paramDTankLevel = 100
                }
                background: Rectangle {
                    border.width: 1
                    border.color: "black"
                    color:  (idPumpTankButton.pressed) ? Design.colButtonSelectedBackground :
                                (idPumpTankButton.hovered) ? Design.colButtonHoveredBackground :
                                    Design.colButtonBackground
                }
                hoverEnabled: true
                ToolTip.delay: 500
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Remet le niveau du réservoir à 100%")
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
                    text: currentVector.funcCleaning().hasReceivedMessage ?
                              currentVector.funcCleaning().paramDTankLevel : ""
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
                color: currentVector.funcCleaning().hasReceivedMessage ?
                           currentVector.funcCleaning().paramDTankLevel > 15 ?
                               "blue" : "#A0A0FF" : "#E5E5E5"
                border.color: "#E5E5E5"
                border.width: 1
                height: 30
                width: 30
            }

            Button {
                id: idCleaningTankButton
                visible: currentVector.funcCleaning().isEnabled
                text: "↑"
                font.pointSize: 14
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                onClicked: {
                    currentVector.funcCleaning().paramDTankLevel = 100
                }
                background: Rectangle {
                    border.width: 1
                    border.color: "black"
                    color:  (idCleaningTankButton.pressed) ? Design.colButtonSelectedBackground :
                                (idCleaningTankButton.hovered) ? Design.colButtonHoveredBackground :
                                    Design.colButtonBackground
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
                    text: currentVector.funcPressureSensor0().hasReceivedMessage ?
                              currentVector.funcPressureSensor0().paramDPressureB / 1000.0 : ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90
            }

            Text {
                id: idDepthType
                text: "↳ " + currentVector.funcPressureSensor0().depthType + " : "
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
                    text: currentVector.funcPressureSensor0().hasReceivedMessage ?
                            currentVector.funcPressureSensor0().depthStrValue : ""
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 12
                }

                height: 30
                width: 90
            }

            Button {
                id: idButtonTare
                visible: currentVector.funcPressureSensor0().isEnabled
                enabled: currentVector.funcPressureSensor0().hasReceivedMessage
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
                background: Rectangle {
                    border.width: 1
                    border.color: "black"
                    color:  (idButtonTare.pressed) ? Design.colButtonSelectedBackground :
                                (idButtonTare.hovered) ? Design.colButtonHoveredBackground :
                                    Design.colButtonBackground
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
                    text: currentVector.funcCompass().hasReceivedMessage ?
                              currentVector.funcCompass().paramDCompassDegr : ""
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
                ToolTip.text: qsTr("Nord = 0° ; Est = 90° ; Sud = 180° ; Ouest = 270°")

                height: 30
                width: 90
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
