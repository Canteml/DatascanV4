import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Item {

    property var componentId: 0
    property var componentName: ""
    property alias idComponentName: idComponentName
    property alias idRectConnectionState: idRectConnectionState
    property alias idBtComponentLinkConnect: idBtComponentLinkConnect
    property alias idComponentLinkIp: idComponentLinkIp
    property alias idCbComponentLinkComPort: idCbComponentLinkComPort
    property alias idCbComponentLinkType: idCbComponentLinkType

    Layout.fillWidth: true
    Layout.minimumHeight: 50

    RowLayout {

        anchors.fill: parent

        Text {
            id: idComponentName
            text: componentName
            font.pixelSize: 14
            Layout.minimumWidth: 60
        }
        Text {
            id: idComponentNameSuffix
            text: " : "
            font.pixelSize: 14
        }
        Rectangle {
            id: idRectConnectionState
            property var stateColor: "#FF0000"

            Layout.minimumHeight: 20
            Layout.minimumWidth: 20

            color: stateColor

            border.color: "black"
            border.width: 1
            radius: 10
        }

        ComboBox {
            id: idCbComponentLinkType
            Layout.maximumWidth: 120
            model: [qsTr("Port Com"), qsTr("IP")]
        }

        ComboBox {
            id: idCbComponentLinkComPort
            Layout.maximumWidth: 120
            model: [qsTr("COM28"), qsTr("COM29")]
            visible: idCbComponentLinkType.currentIndex === 0
        }

        TextInput {
            id: idComponentLinkIp
            Layout.minimumWidth: 120
            text: qsTr("192.168.123.456")
            font.pixelSize: 14
            horizontalAlignment: Text.AlignRight
            visible: idCbComponentLinkType.currentIndex === 1
            validator: RegExpValidator {
                regExp: /((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\.|$)){4}/
            }
        }

        Button {
            id: idBtComponentLinkConnect
            text: qsTr("Connecter")
        }

        // -- Spacer
        Item {
            Layout.fillWidth: true
        }
    }
}
