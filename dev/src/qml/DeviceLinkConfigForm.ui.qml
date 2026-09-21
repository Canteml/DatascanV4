import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Item {
    property var linkName: ""
    property int linkState: 0
    property alias idLinkName: idLinkName
    property alias idRectConnectionState: idRectConnectionState
    property alias idBtLinkConnect: idBtLinkConnect
    property alias idLinkIp: idLinkIp
    property alias idCbLinkComPort: idCbLinkComPort
    property alias idCbLinkType: idCbLinkType

    Layout.fillWidth: true
    Layout.minimumHeight: 40
    height: 40

    onLinkStateChanged: linkState
                        === 0 ? idRectConnectionState.stateColor = "#FFFFFF" : linkState
                                === 1 ? idRectConnectionState.stateColor = "#FFFF00" : linkState
                                        === 2 ? idRectConnectionState.stateColor
                                                = "#00FF00" : idRectConnectionState.stateColor
                                                = "#FF0000"

    RowLayout {

        anchors.fill: parent

        Text {
            id: idLinkName
            text: linkName
            font.pixelSize: 14
            Layout.minimumWidth: 60
        }
        Text {
            id: idLinkNameSuffix
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
            id: idCbLinkType
            Layout.maximumWidth: 120
            Layout.preferredHeight: 30
            model: [qsTr("Port Com"), qsTr("IP")]
        }

        ComboBox {
            id: idCbLinkComPort
            Layout.maximumWidth: 120
            Layout.preferredHeight: 30
            //model: [qsTr("COM28"), qsTr("COM29")]
            visible: idCbLinkType.currentIndex === 0
        }

        TextInput {
            id: idLinkIp
            Layout.minimumWidth: 120
            Layout.preferredHeight: 30
            text: qsTr("192.168.123.456")
            font.pixelSize: 14
            horizontalAlignment: Text.AlignRight
            visible: idCbLinkType.currentIndex === 1
        }

        Button {
            id: idBtLinkConnect
            Layout.preferredHeight: 30
            text: qsTr("Connecter")
        }

        // -- Spacer
        Item {
            Layout.fillWidth: true
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

