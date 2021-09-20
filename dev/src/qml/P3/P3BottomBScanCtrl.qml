import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    color: "#EFEFEF"
    border.color: "#000000"

    ColumnLayout {
        id: idColBScanCtrl
        anchors.fill: parent

//        Layout.maximumWidth: 100

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Echelle")
            font.pixelSize: 16
            Layout.topMargin: 5
        }

        Button {
            id: idBtBScanAuto
            text : "100 %"
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            font.pointSize: 8
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            background: Rectangle {
                color: (idBtBScanAuto.pressed || (scanManager.BScanViewXMode === 2 && scanManager.BScanViewYMode === 2)) ?
                           Design.colBlueSelected : idBtBScanAuto.hovered ? Design.colBlueHovered : Design.colButtonBackground
                border.width: 1
                border.color: Design.colFrameBorder
            }

            onPressed: {
                scanManager.BScanViewXMode = 2; // 100%
                scanManager.BScanViewYMode = 2; // 100%
            }
        }

        Button {
            id: idBtBScanXAuto
            text : "X Auto"
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            font.pointSize: 8
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            background: Rectangle {
                color: (idBtBScanXAuto.pressed || scanManager.BScanViewXMode === 0) ?
                           Design.colBlueSelected : idBtBScanXAuto.hovered ? Design.colBlueHovered : Design.colButtonBackground
                border.width: 1
                border.color: Design.colFrameBorder
            }

            onPressed: {
                scanManager.BScanViewXMode = 0; // AUTO
            }
        }

        Button {
            id: idBtBScanYAuto
            text : "Y Auto"
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            font.pointSize: 8
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            background: Rectangle {
                color: (idBtBScanYAuto.pressed || scanManager.BScanViewYMode === 0) ? Design.colBlueSelected : idBtBScanYAuto.hovered ? Design.colBlueHovered : Design.colButtonBackground
                border.width: 1
                border.color: Design.colFrameBorder
            }

            onPressed: {
                scanManager.BScanViewYMode = 0; // AUTO
            }
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.topMargin: 5
            //Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: "Zones"
            font.pixelSize: 16
        }

        RowLayout {
            spacing: 0
            Layout.leftMargin: 5

            Button {
                id: idBtBScanZoneRemove
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: (idBtBScanZoneRemove.pressed) ? Design.colButtonSelectedBackground : idBtBScanZoneRemove.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                text: "-"
                font.pointSize: 12

                onPressed: {
                    //scanManager.ascanZoomOut()
                }
            }

            Button {
                id: idBtBScanZoneAdd
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: (idBtBScanZoneAdd.pressed) ? Design.colButtonSelectedBackground : idBtBScanZoneAdd.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                text: "+"
                font.pointSize: 12

                onPressed: {
                    //scanManager.ascanZoomIn()
                }
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }
}
