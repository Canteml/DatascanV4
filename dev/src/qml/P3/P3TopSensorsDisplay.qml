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
                id: idPressureLabel
                text: qsTr("Non connecté")
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
                id: idDepthLabel
                text: qsTr("Non connecté")
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
                currentVector.funcPressureSensor0().calibrateDepth0();
            }
        }

        Text {
            text: qsTr("Température :")
            Layout.row: 1
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
                text: qsTr("Non connecté")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }


        Text {
            text: qsTr("Humidité :")
            Layout.row: 2
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
                text: qsTr("Non connecté")
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
                font.bold: true
            }

            height: 50

        }

        Connections {
            target: currentVector.funcPressureSensor0()
            function onParamDPressureBChanged(pressure) {
                idPressureLabel.text = (pressure / 1000.0) + " bar"
            }
        }

        Connections {
            target: currentVector.funcPressureSensor0()
            function onDepthStrValueChanged(depth) {
                idDepthLabel.text = depth
            }
        }


        Connections {
            target: currentVector.funcHumiditySensor0()
            function onEvUpdateHumidity(humidity) {
                idHumidityLabel.text = (humidity / 10.0) + " %"
            }
        }

        Connections {
            target: currentVector.funcTemperatureSensor0()
            function onEvUpdateTemperature(temperature) {
                idTemperatureLabel.text = (temperature / 10.0) + " °C"
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
