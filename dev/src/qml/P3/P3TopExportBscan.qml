import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.1
import Design 1.0

ColumnLayout {
    id: idP3TopP1
    Layout.fillWidth: true
    Layout.fillHeight: true

    property alias exportStep: idSLExport.currentIndex

    StackLayout {
        id: idSLExport

        Connections {
            target: examManager
            function onEvExportTerminatedChanged(p_success) {
                if( p_success ) {
                    idSLExport.updateGeneratrixNumber();
                    idSLExport.currentIndex = 2;
                }
            }
        }

        onCurrentIndexChanged: {
            // init pages at index change
            if( 1 === currentIndex ) {
                idTxtFileName.checkFileName
            }
        }

        // -- PAGE 0
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: exportStep === 0

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

                // Button Cancel
                Button {
                    id: btCancel

                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 200

                    hoverEnabled: true
                    ToolTip.text: qsTr("Supprimer l'acquisition et la recommencer.")
                    ToolTip.delay: 1000
                    ToolTip.visible: hovered

                    icon.source: "qrc:///img/icons/iconCancel.png"
                    icon.width: 150
                    icon.height: 150
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        color: btCancel.pressed ? Design.colButtonSelectedBackground : btCancel.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 40
                    }

                    onPressed: {
                        examManager.selectCurrentGeneratrix()
                        examManager.examStep = 0; // STOPPED
                    }
                }


                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                // Button ok
                Button {
                    id: btOk

                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 200

                    hoverEnabled: true
                    ToolTip.text: qsTr("Exporter l'acquisition.")
                    ToolTip.delay: 1000
                    ToolTip.visible: hovered

                    icon.source: enabled ? "qrc:///img/icons/iconOk.png" : "qrc:///img/icons/iconOkDisabled.png"
                    icon.width: 150
                    icon.height: 150
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        color: btOk.pressed ? Design.colButtonSelectedBackground : btOk.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 40
                    }

                    MessageDialog {
                        id: dialogFileExists
                        title: qsTr("Fichier CSV existant")
                        standardButtons: Dialog.Yes | Dialog.Cancel
                        onAccepted: {
                            examManager.startExamWrite(idTxtFileName.text)
                            txtExamFileName.text = examManager.getExportFullFileName()
                            idSLExport.currentIndex = 1;
                        }
                    }

                    onPressed: {
                        var err = examManager.checkFileName(idTxtFileName.text)
                        if( 0 === err ) {
                            examManager.startExamWrite(idTxtFileName.text)
                            txtExamFileName.text = examManager.getExportFullFileName()
                            idSLExport.currentIndex = 1;
                        } else if( 1 === err ) {
                            idTxtFileName.checkFileName();
                        } else {
                            dialogFileExists.text = qsTr("Voulez-vous ecraser le fichier suitvant : \n")+examManager.makeExamFullFileName(idTxtFileName.text)
                            dialogFileExists.visible = true
                        }
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

            RowLayout {
                Layout.fillWidth: true

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("Nom du CSV : ")
                    font.bold: true
                    font.pointSize: 12
                }

                TextField {
                    id: idTxtFileName
                    selectByMouse: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: idP3Top.width/3
                    font.pointSize: 12


                    Component.onCompleted: {
                        idTxtFileName.text = examManager.bscanPerimeter.toFixed(2)
                    }

                    Connections {
                        target : examManager
                        function onExamStepChanged() {
                            idTxtFileName.text = examManager.getExamGeneratedFileName();
                        }
                    }

                    Connections {
                        target : examManager
                        function onExportFolderChanged() {
                            idTxtFileName.checkFileName();
                        }
                    }

                    onTextChanged: {
                        checkFileName()
                    }

                    function checkFileName() {
                        var err = examManager.checkFileName(idTxtFileName.text)
                        if( 1 === err ) {
                            idTxtFileNameErrorMsg.text = qsTr("Dossier d'export non valide : ")+examManager.exportFolder
                        } else if( 2 === err ) {
                            idTxtFileNameErrorMsg.text = qsTr("Le fichier existe déjà : ")+examManager.makeExamFullFileName(idTxtFileName.text)
                        } else {
                            idTxtFileNameErrorMsg.text = ""
                        }
                        //btOk.enabled = ( 1 !== err );
                    }

                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Layout.fillWidth: true

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                Text {
                    id: idTxtFileNameErrorMsg
                    text: ""
                    font.bold: true
                    color: "#AA2020"
                    font.pointSize: 12
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

        // -- STEP 1 : WRITE PROGRESS
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: exportStep === 1

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
                    id: txtExamFileName
                    font.pointSize: 14
                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }
            }


            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                ProgressBar {
                    id: idPbExport
                    Layout.preferredWidth: 400
                    Layout.minimumHeight: 50
                    from: 0
                    to: 100
                    value: examManager.examWriteProgress

                    background: Rectangle {
                        implicitWidth: 400
                        implicitHeight: 50
                        color: Design.colButtonHoveredBackground
                        radius: 3
                    }

                    contentItem: Item {
                        implicitWidth: 400
                        implicitHeight: 48

                        Rectangle {
                            width: idPbExport.visualPosition * parent.width
                            height: parent.height
                            radius: 2
                            color: Design.colOrangeSelected
                        }
                    }
                }

                Button {
                    id: idBtCancelExport
                    text: qsTr("Annuler")
                    Layout.minimumHeight: 50
                    font.pointSize: 14

                    visible: examManager.examWriteProgress >= 0

                    background: Rectangle {
                        color: idBtCancelExport.pressed ? Design.colButtonSelectedBackground : idBtCancelExport.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 5
                    }

                    onClicked: {
                        examManager.cancelExamWrite()
                    }
                }

                // Button Cancel
                Button {
                    id: btWriteErrorCancel
                    visible: examManager.examWriteProgress === -1

                    Layout.preferredWidth: 50
                    Layout.preferredHeight: 50

                    hoverEnabled: true
                    ToolTip.text: qsTr("Revenir à l'écran d'enregistrement du rapport")
                    ToolTip.delay: 1000
                    ToolTip.visible: hovered

                    icon.source: "qrc:///img/icons/iconCancel.png"
                    icon.width: 40
                    icon.height: 40
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        color: btWriteErrorCancel.pressed ? Design.colButtonSelectedBackground : btWriteErrorCancel.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 10
                    }

                    onPressed: {
                        idSLExport.currentIndex = 0
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

        // -- STEP 2 : WRITE DONE

        function updateGeneratrixNumber() {
            var s = "";
            if( examManager.getGeneratrixNumberPrevious()+1 < 10 ) {
                s = "0"
            } else {
                s = ""
            }
            idTxtPreviousVirole.text =  qsTr("GAUCHE")+" (#"+s+(examManager.getGeneratrixNumberPrevious()+1)+")";
            if( examManager.getGeneratrixNumberNext()+1 < 10 ) {
                s = "0"
            } else {
                s = ""
            }
            idTxtNextVirole.text =  qsTr("DROITE")+" (#"+s+(examManager.getGeneratrixNumberNext()+1)+")";
            if( examManager.selectedBScanGeneratrix+1 < 10 ) {
                s = "0"
            } else {
                s = ""
            }
            idTxtCurrentVirole.text =  qsTr("ACTUELLE")+" (#"+s+(examManager.selectedBScanGeneratrix+1)+")";
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: exportStep === 1

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

            Text {
                Layout.fillWidth: true
                text: qsTr("Choix de la prochaine virole :")
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

                    // Button Previous
                    Button {
                        id: btPrevious
                        Layout.alignment: Qt.AlignHCenter

                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 200

                        icon.source: "qrc:///img/icons/iconPrevious.png"
                        icon.width: 150
                        icon.height: 150
                        icon.color: "transparent"

                        antialiasing: true

                        background: Rectangle {
                            color: btPrevious.pressed ? Design.colButtonSelectedBackground : btPrevious.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
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
                        id: idTxtPreviousVirole
                        text: "GAUCHE"
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

                    // Button Next
                    Button {
                        id: btAgain
                        Layout.alignment: Qt.AlignHCenter

                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 200

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
                        id: idTxtCurrentVirole
                        text: "ACTUELLE"
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
                        id: btNext
                        Layout.alignment: Qt.AlignHCenter

                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 200

                        icon.source: "qrc:///img/icons/iconNext.png"
                        icon.width: 150
                        icon.height: 150
                        icon.color: "transparent"

                        antialiasing: true

                        background: Rectangle {
                            color: btNext.pressed ? Design.colButtonSelectedBackground : btNext.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
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
                        id: idTxtNextVirole
                        text: "DROITE"
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
    }

    // SPACER vertical
    Item {
        Layout.fillHeight: true
    }

}
