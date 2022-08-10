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
        columns: 4
        anchors.fill: parent
        columnSpacing: 20
        anchors.margins: 20

        Text {
            text: qsTr("Pression :")
            visible: currentVector.funcPressureSensor0().isEnabled
            font.pointSize: 11
        }

        Rectangle {
            visible: currentVector.funcPressureSensor0().isEnabled
            border.color: "#000000"
            border.width: 1
            Layout.fillWidth: true

            Text {
                text: qsTr("2 bar")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }

        Rectangle {
            visible: currentVector.funcPressureSensor0().isEnabled
            border.color: "#000000"
            border.width: 1
            Layout.fillWidth: true

            Text {
                text: qsTr("40 m")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }

        Button {
            visible: currentVector.funcPressureSensor0().isEnabled
            text: qsTr("Tare")
            font.pointSize: 12
            Layout.preferredHeight: 50
            onClicked: {
                currentVector.funcPressureSensor0()
            }
        }

        Text {
            text: qsTr("Température :")
            visible: currentVector.funcTemperatureSensor0().isEnabled
            font.pointSize: 11
        }

        Rectangle {
            visible: currentVector.funcTemperatureSensor0().isEnabled
            border.color: "#000000"
            border.width: 1
            Layout.fillWidth: true

            Text {
                id: idTemperatureLabel
                text: qsTr("50 °C")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }

        Rectangle {
            id: idTemperatureGauge
            visible: currentVector.funcTemperatureSensor0().isEnabled
            Layout.fillWidth: true
            Layout.columnSpan: 2
            height: 50
            border.color: "#000000"
            border.width: 1

            LinearGradient {
                anchors.fill: parent
                anchors.margins: 1
                start: Qt.point(1, 1)
                end: Qt.point(idTemperatureGauge.width - 1, 1)
                gradient: Gradient {
                    GradientStop { position: 0.0 ; color: "green" }
                    GradientStop { position: 0.5 ; color: "#CAE400" }
                    GradientStop { position: 0.75 ; color: "orange" }
                    GradientStop { position: 1.0 ; color: "red" }
                }
            }

            Rectangle {
                id: idTemperatureCursor
                width: 3
                color: "black"
                height: 50
                x: parent.width * 0.5
            }

        }

        Text {
            text: qsTr("Humidité :")
            visible: currentVector.funcHumiditySensor0().isEnabled
            font.pointSize: 11
        }

        Rectangle {
            visible: currentVector.funcHumiditySensor0().isEnabled
            border.color: "#000000"
            border.width: 1
            Layout.fillWidth: true

            Text {
                id: idHumidityLabel
                text: qsTr("30 %")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }

        Rectangle {
            id: idHumidityGauge
            visible: currentVector.funcHumiditySensor0().isEnabled
            Layout.fillWidth: true
            Layout.columnSpan: 2
            height: 50
            border.color: "#000000"
            border.width: 1

            LinearGradient {
                anchors.fill: parent
                anchors.margins: 1
                start: Qt.point(1, 1)
                end: Qt.point(idTemperatureGauge.width - 1, 1)
                gradient: Gradient {
                    GradientStop { position: 0.0 ; color: "green" }
                    GradientStop { position: 0.5 ; color: "#CAE400" }
                    GradientStop { position: 0.75 ; color: "orange" }
                    GradientStop { position: 1.0 ; color: "red" }
                }
            }

            Rectangle {
                id: idHumidityCursor
                width: 3
                color: "black"
                height: 50
                x: parent.width * 0.3
            }

        }

        Connections {
            target: currentVector.funcHumiditySensor0()
            function onParamDHumidityCChanged(humidity) {
                idHumidityLabel.text = humidity + " %"
                idHumidityCursor.x = idHumidityGauge.width * (humidity / 100)
            }
        }

        Connections {
            target: currentVector.funcTemperatureSensor0()
            function onParamDTemperatureCChanged(raw_temperature) {
                var temperature = raw_temperature / 10
                idTemperatureLabel.text = temperature + " °C"
                idTemperatureCursor.x = idTemperatureGauge.width * (temperature / 200)
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
