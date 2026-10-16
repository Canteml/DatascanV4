import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Window 2.2

import Qt.labs.qmlmodels 1.0

import Fonts 1.0
import Design 1.0

Rectangle {
    id: idP3Bottom

    // StackLayout controlled by P3SettingsBar / P3Top, set externally
    property var slCamSettings: null

    Layout.fillWidth: true
    Layout.fillHeight : true

    // Full-window catcher, active only while a "Valider ?" is shown: a press
    // anywhere outside the confirm/toggle buttons cancels the pending removal.
    // The press is not accepted, so it still reaches the item underneath.
    MouseArea {
        id: idCancelConfirm
        parent: Window.window ? Window.window.contentItem : idP3Bottom
        anchors.fill: parent
        z: 10000
        enabled: confirmRemoveRow.visible || confirmRemoveCol.visible

        function isOver(item, mouse) {
            if (!item.visible)
                return false
            var p = item.mapFromItem(idCancelConfirm, mouse.x, mouse.y)
            return item.contains(p)
        }

        onPressed: {
            mouse.accepted = false
            if (isOver(confirmRemoveRow, mouse) || isOver(confirmRemoveCol, mouse)
                    || isOver(removeRow, mouse) || isOver(removeCol, mouse))
                return
            confirmRemoveRow.visible = false
            confirmRemoveCol.visible = false
        }
    }

    StackLayout {
        id: idSlBottomMain
        anchors.fill: parent

        // Page 1 (A-SCAN settings) / Page 2 (B-SCAN settings) only while that button is the active selection
        currentIndex: (slCamSettings && slCamSettings.visible && slCamSettings.currentIndex === 1) ? 1
                    : (slCamSettings && slCamSettings.visible && slCamSettings.currentIndex === 2) ? 2
                    : (slCamSettings && slCamSettings.visible && slCamSettings.currentIndex === 3) ? 3
                    : (slCamSettings && slCamSettings.visible && slCamSettings.currentIndex === 4) ? 4
                    : 0

        // --------------------------------------------------------------------
        // -- PAGE 0 : NORMAL VIEW (BScan / MEP / Pipe, driven by exam control type)
        RowLayout {
            spacing: 0

            // Stack function to Exam control type
            StackLayout {
                id: idSlBottom
                currentIndex: examManager.controlType
                Layout.fillWidth: true

                // ----------------------------------------------------------------
                // -- PAGE BSCAN
                RowLayout {
                    id: idBScan
                    spacing: 0
                    Layout.fillWidth: true

                    Rectangle {
                        border.width: 1
                        border.color: "#000000"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        P3BottomBScanView {
                            anchors.fill : parent
                            anchors.margins: 1
                        }
                    }
                }

                // ----------------------------------------------------------------
                // -- PAGE MEP

                RowLayout {
                    id: idMEP
                    spacing: 0
                    Layout.fillWidth: true

                    // Left panel (MEP only)
                    Rectangle {
                        Layout.preferredWidth: 115
                        Layout.fillHeight: true
                        color: "white"
                        border.width: 1
                        border.color: "black"


                        GridLayout {
                            id: idMepCtrlGrid
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 5
                            columns: 2
                            rows: 6
                            columnSpacing: 5
                            rowSpacing: 5

                            Button {
                                Layout.column: 0
                                Layout.row: 0
                                contentItem: Image {
                                    source: "qrc:///img/icons/add_line_up.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                onPressed: {
                                    examManager.addMepRowBefore()
                                }
                            }

                            Button {
                                Layout.column: 1
                                Layout.row: 0
                                contentItem: Image {
                                    source: "qrc:///img/icons/new_column_left.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                onPressed: {
                                    examManager.addMepColBefore()
                                }
                            }

                            Button {
                                Layout.column: 0
                                Layout.row: 1
                                contentItem: Image {
                                    source: "qrc:///img/icons/add_line_down.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                onPressed: {
                                    examManager.addMepRowAfter()
                                }
                            }

                            Button {
                                Layout.column: 1
                                Layout.row: 1
                                contentItem: Image {
                                    source: "qrc:///img/icons/new_column_right.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                onPressed: {
                                    examManager.addMepColAfter()
                                }
                            }

                            Button {
                                Layout.column: 0
                                Layout.row: 2
                                id: removeRow
                                contentItem: Image {
                                    source: "qrc:///img/icons/delite_line.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                enabled : examManager.mepNbRow > 1
                                onPressed: {
                                    confirmRemoveCol.visible = false
                                    confirmRemoveRow.visible = !confirmRemoveRow.visible
                                }
                            }

                            Button {
                                Layout.column: 1
                                Layout.row: 2
                                id: removeCol
                                contentItem: Image {
                                    source: "qrc:///img/icons/delite_column.png"
                                    sourceSize.width: 40
                                    sourceSize.height: 40
                                    fillMode: Image.PreserveAspectFit
                                }
                                font.pointSize: 12
                                enabled : examManager.mepNbCol > 1
                                onPressed: {
                                    confirmRemoveRow.visible = false
                                    confirmRemoveCol.visible = !confirmRemoveCol.visible
                                }
                            }

                            // Fixed slot for the confirm buttons, always reserved so that
                            // showing "Valider ?" never shifts the zoom buttons below.
                            Item {
                                Layout.column: 0
                                Layout.row: 3
                                Layout.columnSpan: 2
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40

                                Button {
                                    id: confirmRemoveRow
                                    anchors.centerIn: parent
                                    visible: false
                                    text: qsTr("Valider ?")
                                    palette.buttonText: "white"
                                    font.pointSize: 12
                                    enabled : examManager.mepNbRow > 1
                                    onPressed: {
                                        examManager.removeCurrentMepRow()
                                        confirmRemoveRow.visible = false
                                    }
                                    background: Rectangle {
                                        implicitWidth: 100
                                        implicitHeight: 40
                                        color: "red"
                                    }
                                }

                                Button {
                                    id: confirmRemoveCol
                                    anchors.centerIn: parent
                                    visible: false
                                    text: qsTr("Valider ?")
                                    palette.buttonText: "white"
                                    font.pointSize: 12
                                    enabled : examManager.mepNbCol > 1
                                    onPressed: {
                                        examManager.removeCurrentMepCol()
                                        confirmRemoveCol.visible = false
                                    }
                                    background: Rectangle {
                                        implicitWidth: 100
                                        implicitHeight: 40
                                        color: "red"
                                    }
                                }
                            }

                            Text {
                                Layout.column: 0
                                Layout.row: 4
                                Layout.columnSpan: 2
                                Layout.alignment: Qt.AlignHCenter
                                text: qsTr("ZOOM")
                                font.family: Fonts.robotoCondensed
                                font.pointSize: 12
                                font.bold: true
                                color: Design.colButtonDarkBackground
                            }

                            Button {
                                Layout.column: 0
                                Layout.row: 5
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                Layout.alignment: Qt.AlignHCenter

                                background: Rectangle {
                                    color: "white"
                                    border.width: 2
                                    border.color: Design.colFrameBorder
                                }

                                contentItem: Text {
                                    text: qsTr("-")
                                    font.family: Fonts.robotoCondensed
                                    font.pointSize: 12
                                    font.bold: true
                                    color: Design.colButtonDarkBackground
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onPressed: {
                                    examManager.mepZoom -= 0.1
                                }
                            }

                            Button {
                                Layout.column: 1
                                Layout.row: 5
                                Layout.preferredHeight: 40
                                Layout.preferredWidth: 40
                                font.pointSize: 12
                                Layout.alignment: Qt.AlignHCenter

                                background: Rectangle {
                                    color: "white"
                                    border.width: 2
                                    border.color: Design.colFrameBorder
                                }

                                contentItem: Text {
                                    text: qsTr("+")
                                    font.family: Fonts.robotoCondensed
                                    font.pointSize: 12
                                    font.bold: true
                                    color: Design.colButtonDarkBackground
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onPressed: {
                                    examManager.mepZoom += 0.1
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: idMep
                        color: "#C9C9C9"
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        P3BottomMepView {
                            anchors.fill: parent
                            anchors.margins: 1
                        }

                    }

                }

                // ----------------------------------------------------------------
                // -- PAGE PIPE
                RowLayout {
                    id: idPipe // panda vol 3
                    spacing: 0
                    Layout.fillWidth: true

                    P3BottomPipeCtrl {
                        Layout.preferredWidth: 70
                        Layout.fillHeight: true
                    }

                    Rectangle {
                        border.width: 1
                        border.color: "#000000"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        P3BottomPipeView {
                            anchors.fill : parent
                            anchors.margins: 1
                        }
                    }
                }
            }
        }

        // --------------------------------------------------------------------
        // -- PAGE 1 : A-SCAN SETTINGS (new screen, placeholder for now)
        P3BottomSettingsAScan {
            slCamSettings: idP3Bottom.slCamSettings
        }

        // --------------------------------------------------------------------
        // -- PAGE 2 : B-SCAN SETTINGS
        P3BottomSettingsBScan {
            slCamSettings: idP3Bottom.slCamSettings
        }

        // --------------------------------------------------------------------
        // -- PAGE 3 : MEP SETTINGS
        P3BottomSettingsMep {
            slCamSettings: idP3Bottom.slCamSettings
        }

        // --------------------------------------------------------------------
        // -- PAGE 4 : MEP SETTINGS
        P3BottomSensorsDisplay {
            slCamSettings: idP3Bottom.slCamSettings
        }
    }
}
