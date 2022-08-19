import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.11
import Design 1.0
import QtGraphicalEffects 1.0
import "../rp_components"

Rectangle {

    color: "#FFFFFF"
    border.width: 1


    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            height: 35

            RowLayout{
                anchors.fill: parent
                spacing: 0

                Button {
                    id: idBtTable
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onPressed: {
                        idMepStack.currentIndex = 0
                    }
                    background: Rectangle {
                        color: (idBtTable.pressed
                                || (idMepStack.visible
                                    && idMepStack.currentIndex
                                    == 0)) ? Design.colButtonSelectedBackground : idBtTable.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }
                    contentItem: Text {
                        text: qsTr("VUE TABLEUR")
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: idBtComments
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onPressed: {
                        idMepStack.currentIndex = 1
                    }
                    background: Rectangle {
                        color: (idBtComments.pressed
                                || (idMepStack.visible
                                    && idMepStack.currentIndex
                                    == 1)) ? Design.colButtonSelectedBackground : idBtComments.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }
                    contentItem: Text {
                        text: qsTr("ANNOTATIONS")
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        StackLayout {
            id: idMepStack
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 10

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    rows: 2
                    columns: 6

                    Text {
                        text: qsTr("Ajouter une ligne :")
                        font.pointSize: 12
                    }

                    Button {
                        text: qsTr("Au dessus")
                        font.pointSize: 12
                        onPressed: {
                            examManager.addMepRowBefore()
                        }
                    }

                    Button {
                        text: qsTr("En dessous")
                        font.pointSize: 12
                        onPressed: {
                            examManager.addMepRowAfter()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: qsTr("Ajouter une colonne :")
                        font.pointSize: 12
                        Layout.row: 1
                    }

                    Button {
                        text: qsTr("A gauche")
                        font.pointSize: 12
                        onPressed: {
                            examManager.addMepColBefore()
                        }
                    }

                    Button {
                        text: qsTr("A droite")
                        font.pointSize: 12
                        onPressed: {
                            examManager.addMepColAfter()
                        }
                    }

                    Text {
                        text: qsTr("Ligne actuelle :")
                        Layout.row: 2
                        font.pointSize: 12
                    }

                    Button {
                        text: qsTr("Supprimer")
                        font.pointSize: 12
                        onPressed: {
                            examManager.removeCurrentMepRow()
                        }
                    }

                    Text {
                        text: qsTr("Colonne actuelle :")
                        Layout.row: 3
                        font.pointSize: 12
                    }

                    Button {
                        text: qsTr("Supprimer")
                        font.pointSize: 12
                        onPressed: {
                            examManager.removeCurrentMepCol()
                        }
                    }

                    Text {
                        text: qsTr("Zoom :")
                        font.pointSize: 12
                        Layout.row: 4
                    }

                    Button {
                        text: qsTr("+")
                        font.pointSize: 12
                        onPressed: {
                            examManager.mepZoom += 0.1
                        }
                    }

                    Button {
                        text: qsTr("-")
                        //enabled: examManager.mepZoom > 0.1
                        font.pointSize: 12
                        onPressed: {
                            examManager.mepZoom -= 0.1
                        }
                    }

                }

                // spacer
                Item {
                    Layout.fillHeight: true
                }

            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

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
                    }
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
                    }
                }

            }

        }
    }



}
