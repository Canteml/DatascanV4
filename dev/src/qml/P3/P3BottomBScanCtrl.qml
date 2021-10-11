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

        Button {
            id: idBtMes1E
            text : qsTr("1 ECHO")
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            font.pointSize: 8
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.topMargin: 5

            background: Rectangle {
                color: (idBtMes1E.pressed || examManager.MeasureDisplay === 0 /*MEASURE_1E*/ ) ? "#E56767" : idBtMes1E.hovered ? "#F4ADAD" : Design.colButtonBackground
                border.width: 1
                border.color: Design.colFrameBorder
            }

            onPressed: {
                if( examManager.MeasureDisplay === 0 /*MEASURE_1E*/ ) {
                    examManager.MeasureDisplay = 1; /* MEASURE_ME */
                } else {
                    examManager.MeasureDisplay = 0;
                }
            }
        }

        ColumnLayout {
            id: idColBScanCtrlRecording
            Layout.fillWidth: true
            visible: examManager.examStep !== 3 /*EXAM_EXPORTING*/

            Text {
                Layout.alignment: Qt.AlignCenter
                text: qsTr("Echelle")
                font.pixelSize: 16
                Layout.topMargin: 5
            }

            Button {
                id: idBtBScanXAuto
                text : qsTr("X Auto")
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pointSize: 8

                Layout.leftMargin: 5
                Layout.rightMargin: 5

                background: Rectangle {
                    color: (idBtBScanXAuto.pressed || scanManager.BScanViewXMode === 0) ?
                               Design.colOrange : idBtBScanXAuto.hovered ? Design.colOrangeHovered : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                onPressed: {
                    if( scanManager.BScanViewXMode !== 0 ) {
                        scanManager.BScanViewXMode = 0; // AUTO
                    } else {
                        scanManager.BScanViewXMode = 1; // USER
                    }
                }
            }

            Button {
                id: idBtBScanYAuto
                text : qsTr("Y Auto")
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pointSize: 8
                Layout.leftMargin: 5
                Layout.rightMargin: 5

                background: Rectangle {
                    color: (idBtBScanYAuto.pressed || scanManager.BScanViewYMode === 0) ? Design.colOrange: idBtBScanYAuto.hovered ? Design.colOrangeHovered : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                onPressed: {
                    if( scanManager.BScanViewYMode !== 0 ) {
                        scanManager.BScanViewYMode = 0; // AUTO
                    } else {
                        scanManager.BScanViewYMode = 1; // USER
                    }
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
                spacing: 5
                Layout.leftMargin: 5

                Button {
                    id: idBtBScanZoneRemove
                    Layout.preferredHeight: 28
                    Layout.preferredWidth: 28
                    antialiasing: true
    //                background: Rectangle {
    //                    color: (idBtBScanZoneRemove.pressed) ? Design.colButtonSelectedBackground : idBtBScanZoneRemove.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
    //                    border.width: 1
    //                    border.color: Design.colFrameBorder
    //                }

                    background: Rectangle {
                        color: idBtBScanZoneRemove.pressed ? Design.colButtonSelectedBackground : idBtBScanZoneRemove.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colButtonBorder
                        radius: 5
                    }

                    icon.source: "qrc:///img/icons/iconTrash.png"
                    //icon.width: idBtBScanZoneRemove.width
                    //icon.height: idBtBScanZoneRemove.width
                    icon.color: "transparent"
                    padding: -1

                    onPressed: {
                        examManager.zoneRemoveLast();
                    }
                }

                Button {
                    id: idBtBScanZoneAdd
                    Layout.preferredHeight: 28
                    Layout.preferredWidth: 28
                    antialiasing: true
    //                background: Rectangle {
    //                    color: (idBtBScanZoneAdd.pressed) ? Design.colButtonSelectedBackground : idBtBScanZoneAdd.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
    //                    border.width: 1
    //                    border.color: Design.colFrameBorder
    //                }

                    background: Rectangle {
                        color: idBtBScanZoneAdd.pressed ? Design.colButtonSelectedBackground : idBtBScanZoneAdd.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colButtonBorder
                        radius: 5
                    }

                    icon.source: "qrc:///img/icons/iconPlus.png"
                    //icon.width: idBtBScanZoneAdd.width
                    //icon.height: idBtBScanZoneAdd.width
                    icon.color: "transparent"
                    padding: 1

                    onPressed: {
                        examManager.zoneAddToCurrentPosition();
                    }
                }
            }

            Button {
                id: idBtZoneAuto
                text : "Auto"
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pointSize: 8
                Layout.leftMargin: 5
                Layout.rightMargin: 5

                background: Rectangle {
                    color: (idBtZoneAuto.pressed || examManager.zoneAuto === true) ? Design.colOrange : idBtZoneAuto.hovered ? Design.colOrangeHovered : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                onPressed: {
                    examManager.zoneAuto = 1 -examManager.zoneAuto;
                }
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }
}
