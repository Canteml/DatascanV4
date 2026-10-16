import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

// End of a BSCAN acquisition : the CSV has been written while measuring (see ExportCsv).
// Show the file and choose the next generatrix.
ColumnLayout {
    id: idP3TopP1
    Layout.fillWidth: true
    Layout.fillHeight: true

    // called by P3Top when the page is shown
    function init() {
        var file = examManager.getExportFullFileName();
        if( file === "" ) {
            idTxtExamFileName.text = qsTr("Aucun fichier CSV n'a été créé (voir le dossier d'export).")
            idTxtExamFileName.color = "#AA2020"
        } else {
            idTxtExamFileName.text = file
            idTxtExamFileName.color = "black"
        }
        updateGeneratrixNumber();
    }

    function updateGeneratrixNumber() {
        var s = "";
        if( examManager.getGeneratrixNumberPrevious()+1 < 10 ) {
            s = "0"
        } else {
            s = ""
        }
        idTxtLeftGene.text =  qsTr("GAUCHE")+" (#"+s+(examManager.getGeneratrixNumberNext()+1)+")";
        if( examManager.getGeneratrixNumberNext()+1 < 10 ) {
            s = "0"
        } else {
            s = ""
        }
        idTxtRightVirole.text =  qsTr("DROITE")+" (#"+s+(examManager.getGeneratrixNumberPrevious()+1)+")";
        if( examManager.selectedBScanGeneratrix+1 < 10 ) {
            s = "0"
        } else {
            s = ""
        }
        idTxtCurrentGene.text =  qsTr("ACTUELLE")+" (#"+s+(examManager.selectedBScanGeneratrix+1)+")";
    }

    // SPACER vertical
    Item {
        Layout.fillHeight: true
    }

    RowLayout {
        Layout.fillWidth: true

        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }

        Text {
            text: qsTr("Sauvegarde du BSCAN : ")
            font.bold: true
            font.pointSize: 14
        }

        Text {
            id: idTxtExamFileName
            font.pointSize: 14
        }

        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }
    }

    // SPACER vertical
    Item {
        Layout.fillHeight: true
    }

    Text {
        Layout.fillWidth: true
        text: qsTr("Choix de la prochaine génératrice :")
        font.bold: true
        font.pointSize: 12
        horizontalAlignment: Text.AlignHCenter
    }

    // SPACER vertical
    Item {
        Layout.fillHeight: true
    }

    RowLayout {
        Layout.fillWidth: true


        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }

        ColumnLayout {

            spacing: 10

            // Button Left
            Button {
                id: btLeft
                Layout.alignment: Qt.AlignHCenter

                Layout.preferredWidth: 200
                Layout.preferredHeight: 200

                icon.source: "qrc:///img/icons/iconPrevious.png"
                icon.width: 150
                icon.height: 150
                icon.color: "transparent"

                antialiasing: true

                background: Rectangle {
                    color: btLeft.pressed ? Design.colButtonSelectedBackground : btLeft.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 2
                    border.color: Design.colButtonBorder
                    radius: 40
                }

                onPressed: {
                    examManager.selectNextGeneratrix();
                    examManager.examStep = 0; // STOPPED
                }
            }

            Text {
                Layout.fillWidth: true
                id: idTxtLeftGene
                text: qsTr("GAUCHE")
                font.bold: true
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
            }
        }


        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }

        ColumnLayout {

            spacing: 10

            // Button Current
            Button {
                id: btAgain
                Layout.alignment: Qt.AlignHCenter

                Layout.preferredWidth: 200
                Layout.preferredHeight: 200

                hoverEnabled: true
                ToolTip.text: qsTr("Recommencer l'acquisition sur la même génératrice (le fichier CSV est conservé).")
                ToolTip.delay: 1000
                ToolTip.visible: hovered

                icon.source: "qrc:///img/icons/iconAgain.png"
                icon.width: 150
                icon.height: 150
                icon.color: "transparent"

                antialiasing: true

                background: Rectangle {
                    color: btAgain.pressed ? Design.colButtonSelectedBackground : btAgain.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 2
                    border.color: Design.colButtonBorder
                    radius: 40
                }

                onPressed: {
                    examManager.selectCurrentGeneratrix();
                    examManager.examStep = 0; // STOPPED
                }
            }

            Text {
                Layout.fillWidth: true
                id: idTxtCurrentGene
                text: qsTr("ACTUELLE")
                font.bold: true
                font.pointSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }

        ColumnLayout {

            spacing: 10

            // Button Next
            Button {
                id: btRight
                Layout.alignment: Qt.AlignHCenter

                Layout.preferredWidth: 200
                Layout.preferredHeight: 200

                icon.source: "qrc:///img/icons/iconNext.png"
                icon.width: 150
                icon.height: 150
                icon.color: "transparent"

                antialiasing: true

                background: Rectangle {
                    color: btRight.pressed ? Design.colButtonSelectedBackground : btRight.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 2
                    border.color: Design.colButtonBorder
                    radius: 40
                }

                onPressed: {
                    examManager.selectPreviousGeneratrix();
                    examManager.examStep = 0; // STOPPED
                }
            }

            Text {
                Layout.fillWidth: true
                id: idTxtRightVirole
                text: qsTr("DROITE")
                font.bold: true
                font.pointSize: 14
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // SPACER horizontal
        Item {
            Layout.fillWidth: true
        }
    }

    // SPACER vertical
    Item {
        Layout.fillHeight: true
    }
}
