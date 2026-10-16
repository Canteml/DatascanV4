import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "./P3"

Rectangle {
    id: idCentralP3

    Layout.fillWidth: true
    Layout.fillHeight: true

    border.width: 0

    property alias idSlP3SideActionSettings: idSettingsSelection

    QtObject {
        id: idSettingsSelection
        property bool actionVisible: true
    }

    ColumnLayout {
        id: colP3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: vectorControl.visible ? vectorControl.left : idP3RightPanel.left

        spacing: 0

        P3Top {
            id: idP3Top
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        P3Bottom {
            slCamSettings: idP3Top.idSlP4TopCamSettings
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    P3VectorIO {
        id: vectorControl
        visible: idSettingsSelection.actionVisible

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: idP3RightPanel.left
        width: 160

        color: "#FFFFFF"
        border.width: 1
        border.color: "#000000"
    }

    P3SettingsBar {
        id: idP3RightPanel

        slCamSettings: idP3Top.idSlP4TopCamSettings
        slActionSettings: idSettingsSelection

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        width: 34
    }
}
