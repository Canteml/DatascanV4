import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    id: idRectPageSecting

    // Selection state shared with P3Top / P3Bottom (P3Top.idSlP4TopCamSettings), set externally
    property var slCamSettings: null
    // Visibility state of the P3VectorIO side bar (CentralPage3.idSlP3SideActionSettings), set externally
    property var slActionSettings: null

    clip: true

    border.color: Design.colBlackBorder

    RowLayout {

        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: idP4TopCamSettingsBar

            color: "#FFFFFF"

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            ColumnLayout {
                id: idColP4TopCamSettingsBar
                anchors.fill: parent
                spacing: 0

                Button {
                    id: idBtAction
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    background: Rectangle {
                        color: (idBtAction.pressed || (slActionSettings && slActionSettings.actionVisible)) ? Design.colButtonBackground : idBtAction.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("ACTION")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtAction.pressed || (slActionSettings && slActionSettings.actionVisible)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slActionSettings ) return;
                        slActionSettings.actionVisible = !slActionSettings.actionVisible;
                    }
                }

                Button {
                    id: idBtCam
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    background: Rectangle {
                        color: (idBtCam.pressed || (slCamSettings && slCamSettings.camVisible)) ? Design.colButtonBackground : idBtCam.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("CAMERA")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtCam.pressed || (slCamSettings && slCamSettings.camVisible)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.camVisible = !slCamSettings.camVisible;
                    }
                }

                Button {
                    id: idBtAScan
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    background: Rectangle {
                        color: (idBtAScan.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 1)) ? Design.colButtonBackground : idBtAScan.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("A-SCAN")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtAScan.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 1)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.currentIndex === 1 && slCamSettings.visible ? slCamSettings.visible = false : slCamSettings.visible = true;
                        slCamSettings.currentIndex = 1;
                    }
                }

                Button {
                    id: idBtBScan
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    visible: (examManager.controlType === 0) // CONTROL_BSCAN_GENERATRIX
                    background: Rectangle {
                        color: (idBtBScan.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 2)) ? Design.colButtonBackground : idBtBScan.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("B-SCAN")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtBScan.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 2)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.currentIndex === 2 && slCamSettings.visible ? slCamSettings.visible = false : slCamSettings.visible = true;
                        slCamSettings.currentIndex = 2;
                    }
                }

                Button {
                    id: idBtMep
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    visible: (examManager.controlType === 1) // CONTROL_BSCAN_MEP
                    background: Rectangle {
                        color: (idBtMep.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 3)) ? Design.colButtonBackground : idBtMep.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("MEP")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtMep.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 3)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.currentIndex === 3 && slCamSettings.visible ? slCamSettings.visible = false : slCamSettings.visible = true;
                        slCamSettings.currentIndex = 3;
                    }
                }

                Button {
                    id: idBtEtat
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    background: Rectangle {
                        color: (idBtEtat.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 4)) ? Design.colButtonBackground : idBtEtat.hovered ? Design.colButtonHoveredBackground : Design.colFillMenuGrey
                        border.width: 1
                        border.color: Design.colBlackBorder
                    }

                    contentItem: Text {
                        text: qsTr("ETAT")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        rotation: -90
                        color: (idBtEtat.pressed
                                || (slCamSettings && slCamSettings.visible
                                    && slCamSettings.currentIndex === 4)) ? "white" : Design.colMainInk
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.currentIndex === 4 && slCamSettings.visible ? slCamSettings.visible = false : slCamSettings.visible = true;
                        slCamSettings.currentIndex = 4;
                    }
                }
            }
        }
    }
}
