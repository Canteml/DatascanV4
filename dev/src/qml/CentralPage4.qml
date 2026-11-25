import QtQuick 2.15
import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "rp_components"
import Fonts 1.0
import Design 1.0

Rectangle {
    id: idCentralP4

    Layout.fillWidth: true
    Layout.fillHeight: true

    anchors.margins: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5

        /*
        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Licence équipement actuelle")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Utilisation restante : ")
                font.pixelSize: 14
            }

            Text {
                id: idRemainingLicenceLabel
                text: currentVector.funcLicence().hasReceivedMessage ? currentVector.funcLicence().paramDStatusLicenceP : ""
                font.pixelSize: 14
            }

            Text {
                text: qsTr("%")
                font.pixelSize: 14
            }

            Item {
                Layout.fillWidth: true
            }

        }

        Item {
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Demande de nouvelle licence")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Pour demander une nouvelle licence équipement, merci de contacter Roboplanet muni d'une clé d'équipement générée ci-dessous")
                font.pixelSize: 14
            }

        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Clé d'équipement :")
                font.pixelSize: 14
            }

            TextField {
                id: idEquipmentKeyDisplay
                text: ""
                readOnly: true
                selectByMouse: true
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pixelSize: 14
            }

            Button {
                id: idBtGenerateEquipmentKey
                text: qsTr("Générer une clé d'équipement")
                Layout.preferredHeight: 30
                font.pixelSize: 14
                onClicked: currentVector.funcLicence().askForKeyEquipment()
            }

            Button {
                id: idBtCopyEquipmentKey
                text: qsTr("Copier la clé d'équipement")
                Layout.preferredHeight: 30
                enabled: false
                font.pixelSize: 14
                onClicked: clipboard.text = idEquipmentKeyDisplay.text
            }

            Connections {
                target: currentVector.funcLicence()
                function onKeyEquipmentChanged(keyEquipment) {
                    idEquipmentKeyDisplay.text = keyEquipment
                    idBtCopyEquipmentKey.enabled = true
                }
            }

        }

        Item {
            height: 20
        }

        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Activation d'une nouvelle licence équipement")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Clé de licence :")
                font.pixelSize: 14
            }

            TextField {
                id: idNewLicenceKey
                selectByMouse: true
                Layout.fillWidth: true;
                Layout.preferredHeight: 30
                font.pixelSize: 14
            }

            Button {
                text: qsTr("Utiliser la clé")
                Layout.preferredHeight: 30
                font.pixelSize: 14
                onClicked: {
                    currentVector.funcLicence().configureNewLicence(idNewLicenceKey.text)
                }
            }
        }*/

        Item {
            Layout.fillWidth: true
            height: 30
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0
            Rectangle {
                color: "#D5DEE3"
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: 40
                border.width: 1
                border.color: Design.colFrameBorder

                RowLayout {
                    anchors.fill: parent
                    spacing: 5

                    Text {
                        text: qsTr("LICENCE ÉQUIPMENT")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 12
                        font.bold: true
                        color: Design.colMainInk
                        Layout.fillWidth: true
                        Layout.leftMargin: 5
                    }
                }
            }

            Rectangle {
                color: "white"
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                height: 150
                border.width: 1
                border.color: Design.colFrameBorder

                GridLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 15
                    columns: 3
                    columnSpacing: 15
                    rowSpacing: 20

                    Text {
                        text: qsTr("Utilisation restante : ")
                        font.pixelSize: 14
                        font.family: Fonts.robotoCondensed
                        Layout.row: 0
                        Layout.column: 0
                    }

                    RowLayout {
                        Layout.row: 0
                        Layout.column: 1

                        Text {
                            id: idRemainingLicenceLabel
                            text: currentVector.funcLicence().hasReceivedMessage ? currentVector.funcLicence().paramDStatusLicenceP : ""
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                        }

                        Text {
                            text: qsTr("%")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.row: 0
                        Layout.column: 2
                    }

                    Text {
                        text: qsTr("Pour demander une nouvelle licence équipment, merci de contacter Roboplanet muni d'une clé d'équipment générée ci-dessous")
                        font.pixelSize: 14
                        font.family: Fonts.robotoCondensed
                        font.bold: true
                        wrapMode: Text.WordWrap
                        Layout.row: 1
                        Layout.column: 0
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                    }

                    Text {
                        text: qsTr("Clé d'équipement ")
                        font.pixelSize: 14
                        font.family: Fonts.robotoCondensed
                        Layout.row: 2
                        Layout.column: 0
                    }


                    TextField {
                        id: idEquipmentKeyDisplay
                        text: ""
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        font.bold: true
                        color: "black"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        readOnly: true
                        selectByMouse: true
                        Layout.preferredHeight: 40
                        Layout.preferredWidth: 1200
                        Layout.fillWidth: true
                        Layout.row: 2
                        Layout.column: 1

                        background: Rectangle {
                           color: "white"
                           border.width: 2
                           border.color: Design.colFrameBorder
                        }
                    }

                    RowLayout {
                        Layout.row: 2
                        Layout.column: 2

                        Button {
                            id: idBtGenerateEquipmentKey
                            Layout.preferredHeight: 40
                            font.pixelSize: 14
                            onClicked: currentVector.funcLicence().askForKeyEquipment()

                            contentItem: Text {
                                text: qsTr("Générer une clé d'équipement")
                                font.family: Fonts.robotoCondensed
                                font.pixelSize: 14
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: "#0D2B38"
                                border.color: Design.colBlackBorder
                                border.width: 3
                            }
                        }

                        Button {
                            id: idBtCopyEquipmentKey

                            contentItem: Text {
                                text: qsTr("Copier la clé d'équipement")
                                font.family: Fonts.robotoCondensed
                                font.pixelSize: 14
                                color: Design.colMainInk
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            Layout.preferredHeight: 40
                            enabled: false
                            font.pixelSize: 14
                            onClicked: clipboard.text = idEquipmentKeyDisplay.text

                            background: Rectangle {
                               color: "white"
                               border.width: 2
                               border.color: Design.colFrameBorder
                            }
                        }

                        Connections {
                            target: currentVector.funcLicence()
                            function onKeyEquipmentChanged(keyEquipment) {
                                idEquipmentKeyDisplay.text = keyEquipment
                                idBtCopyEquipmentKey.enabled = true
                            }
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            height: 30
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0
            Rectangle {
                color: "#D5DEE3"
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: 40
                border.width: 1
                border.color: Design.colFrameBorder

                RowLayout {
                    anchors.fill: parent
                    spacing: 5

                    Text {
                        text: qsTr("ACTIVATION LICENCE ÉQUIPMENT")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 12
                        font.bold: true
                        color: Design.colMainInk
                        Layout.fillWidth: true
                        Layout.leftMargin: 5
                    }
                }
            }

            Rectangle {
                color: "white"
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                height: 70
                border.width: 1
                border.color: Design.colFrameBorder

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 15

                    Text {
                        text: qsTr("Clé de licence ")
                        font.pixelSize: 14
                        font.family: Fonts.robotoCondensed
                        Layout.row: 0
                        Layout.column: 0
                    }


                    TextField {
                        id: idNewLicenceKey
                        selectByMouse: true
                        text: ""
                        color: "black"
                        font.family: Fonts.robotoCondensed
                        verticalAlignment: Text.AlignVCenter
                        Layout.preferredHeight: 40
                        Layout.preferredWidth: 1270
                        font.pixelSize: 14
                        font.bold: true

                        background: Rectangle {
                           color: "white"
                           border.width: 2
                           border.color: Design.colFrameBorder
                        }
                    }

                    Button {
                        contentItem: Text {
                            text: qsTr("Utiliser la clé")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: Design.colMainInk
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Layout.preferredHeight: 40
                        Layout.preferredWidth: 400
                        background: Rectangle {
                           color: "white"
                           border.width: 2
                           border.color: Design.colFrameBorder
                        }
                        onClicked: {
                            currentVector.funcLicence().configureNewLicence(idNewLicenceKey.text)
                        }
                    }
                }
            }
        }
        RowLayout {
            Layout.margins: 50

            Rectangle {
                Layout.fillWidth: true
                height: 60
                color: "#E5E5E5"

                Text {
                    anchors.fill: parent
                    text: qsTr("Ne pas éteindre l'équipement pendant tout le processus de mise à jour de la licence !")
                    color: "black"
                    font.bold: true
                    font.pixelSize: 18
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

            }

        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }

        BannerBottomNavigate {
            id: idBannerBottom4
            idBtPrev.text : qsTr("SCANNER")
            idBtNext.visible: false

            Connections {
                target: idBannerBottom4.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 3;
                }
            }
        }
    }
}
