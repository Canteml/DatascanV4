import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    id: idRectBottomSettingsAScan

    // Shared state with P3SettingsBar / P3Top, set externally
    property var slCamSettings: null

    color: "white"
    border.width: 1
    border.color: Design.colFrameBorder

    ColumnLayout {
        anchors.fill: parent
        spacing: 0


        Rectangle {
            color: Design.colButtonDarkBackground
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            RowLayout {
                anchors.fill: parent
                spacing: 5

                Button {
                    id: idBtClose
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 5

                    background: Rectangle {
                        color: "white"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("FERMER")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        color: Design.colButtonDarkBackground
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        if( !slCamSettings ) return;
                        slCamSettings.currentIndex = 0;
                    }
                }
            }
        }

        Item {
            id: idColumnsContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: 0

                    Item {
                        Layout.fillWidth: true
                    }

                    ColumnLayout {
                        id: idRhightColumn1
                        Layout.alignment: Qt.AlignTop
                        spacing: 8


                        // Debut POrte 1 Setting
                        // COLUMN 1 - RAW 2

                        Text {
                            text: qsTr("Commentaire global sur l'acquisition :")
                            font.pointSize: 12
                        }

                        ScrollView {
                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            TextArea {
                                id: idGlobalCommentField
                                font.pointSize: 12

                                background: Rectangle {
                                    border.width: 1
                                    color: "white"//"#E5E5E5"
                                }

                                Text {
                                    visible: (!parent.text && !parent.activeFocus)
                                    anchors.fill: parent
                                    text: qsTr("Commentaire global sur l'acquisition")
                                    font.italic: true
                                    color: "#555555"
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pointSize: 12
                                }

                                onTextChanged: {
                                    examManager.examComment = text
                                }
                            }
                        }

                        Text {
                            text: qsTr("Annotations de cellules :")
                            font.pointSize: 12
                        }

                        ScrollView {
                            id: idCellsAnnotationsField
                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            TextArea {
                                font.pointSize: 12

                                background: Rectangle {
                                    border.width: 1
                                    color: "white"//"#E5E5E5"
                                }

                                Text {
                                    visible: (!parent.text && !parent.activeFocus)
                                    anchors.fill: parent
                                    text: qsTr("B,5cm : Mesure impossible...")
                                    font.italic: true
                                    color: "#555555"
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pointSize: 12
                                }

                                onTextChanged: {
                                    examManager.cellsAnnotations = text
                                }
                            }
                        }
                    }
                    Item { Layout.fillWidth: true }
                }
            }
        }
    }
}
