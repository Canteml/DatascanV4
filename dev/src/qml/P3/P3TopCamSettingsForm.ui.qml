import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    id: idRectCamSettings

    property alias idRectCamSettings: idRectCamSettings
    Layout.fillWidth: idSlP4TopCamSettings.visible
    Layout.preferredWidth: 70

    color: "#10A055"

    property alias idBtAScanSettings: idBtAScanSettings
    property alias idBtBScanSettings: idBtBScanSettings
    property alias idSlP4TopCamSettings: idSlP4TopCamSettings
    property alias idBtCam: idBtCam

    RowLayout {

        anchors.fill: parent
        spacing: 0

        StackLayout {
            id: idSlP4TopCamSettings

            currentIndex: 0

            P3TopCam {}
            P3TopSettingsAscan {}
            P3TopSettingsBscan {}
        }

        Rectangle {
            id: idP4TopCamSettingsBar

            color: "#0099BB"

            width: 70
            Layout.fillHeight: true

            ColumnLayout {
                id: idColP4TopCamSettingsBar
                anchors.fill: parent
                spacing: 0

                Button {
                    id: idBtCam
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: (idBtCam.pressed
                                || (idSlP4TopCamSettings.visible
                                    && idSlP4TopCamSettings.currentIndex
                                    == 0)) ? Design.colButtonSelectedBackground : idBtCam.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("CAMERA")
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: idBtAScanSettings
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: (idBtAScanSettings.pressed
                                || (idSlP4TopCamSettings.visible
                                    && idSlP4TopCamSettings.currentIndex
                                    == 1)) ? Design.colButtonSelectedBackground : idBtAScanSettings.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("ASCAN")
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: idBtBScanSettings
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: (idBtBScanSettings.pressed
                                || (idSlP4TopCamSettings.visible
                                    && idSlP4TopCamSettings.currentIndex
                                    == 2)) ? Design.colButtonSelectedBackground : idBtBScanSettings.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("BSCAN")
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

