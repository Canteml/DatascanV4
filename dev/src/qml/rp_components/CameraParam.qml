import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtMultimedia
import Design 1.0

RowLayout {
    id: idRwCamParam

    property var cameraRoleItem
    property var cameraRoleText : ""

    property var txtWidth: 80
    property var btAssignWidth: 100
    property var btPreviewWidth: 100



    visible: cameraRoleItem.isEnabled

    Rectangle {
        width: 14
        height: 14
        radius: 14
        border.color: "#000000"
        border.width: 1
        color: cameraRoleItem.cameraIndex === -1 ? "#FFFFFF" : "#00E000"
    }
    Text {
        Layout.preferredWidth: idRwCamParam.txtWidth
        font.pixelSize: 14
        text: cameraRoleText
    }
    Button {
        id: idBtAssign0
        Layout.preferredWidth: idRwCamParam.btAssignWidth
        background: Rectangle {
            color: idBtAssign0.pressed ? Design.colButtonSelectedBackground : idBtAssign0.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
            border.width: 1
            border.color: Design.colFrameBorder
        }

        contentItem: Text {
            text: qsTr("Assigner")
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onPressed: {
            cameraManager.assignPreviewAs(cameraRoleItem.getRole())
        }
    }

    Button {
        id: idBtPv0
        Layout.preferredWidth: idRwCamParam.btPreviewWidth
        background: Rectangle {
            color: (cameraRoleItem.cameraIndex === cameraManager.previewCameraIndex &&  cameraManager.previewCameraIndex !== -1 ) ? Design.colButtonSelectedBackground : idBtPv0.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
            border.width: 1
            border.color: Design.colFrameBorder
        }

        contentItem: Text {
            text: qsTr("Visualiser")
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onPressed: {
            cameraManager.setPreviewCameraIndex(cameraRoleItem.cameraIndex)
        }
    }
}
