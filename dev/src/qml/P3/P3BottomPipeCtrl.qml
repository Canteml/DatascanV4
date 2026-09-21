import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Dialogs
import Qt.labs.platform 1.1 as Platform
import Design 1.0

Rectangle {
    color: "#EFEFEF"
    border.color: "#000000"

    ColumnLayout {
        id: idColPipeCtrl
        anchors.fill: parent
        anchors.topMargin: 5

        RowLayout {
//            Layout.fillWidth: true;
//            Layout.preferredHeight: 28
            spacing: 5
            Layout.leftMargin: 5

            Button {
                id: idBtPipeNew
                Layout.preferredHeight: 28
                Layout.preferredWidth: 28
                antialiasing: true

                background: Rectangle {
                    color: idBtPipeNew.pressed ? Design.colButtonSelectedBackground : idBtPipeNew.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colButtonBorder
                    radius: 3
                }

                icon.source: "qrc:///img/icons/fileNew.png"
                icon.color: "transparent"
                icon.height: 22
                icon.width: 22
                padding: 1

                onPressed: {
                    if( examPipeManager.isExamModified() ) {
                        idMsdExamModified.visible = true;
                    } else {
                        examPipeManager.resetExam();
                    }
                }

                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Démarrer un nouvel examen.")
            }

            MessageDialog {
                id: idMsdExamModified
                //icon: Std.StandardIcon.Warning
                title: qsTr("Examen non sauvegardé !")
                text: qsTr("Des données n'ont pas été enregistrées.\nVoulez-vous quand même démarrer un nouvel examen ?")
                buttons: MessageDialog.Yes | MessageDialog.No
                onAccepted: examPipeManager.resetExam();
            }

            Button {
                id: idBtPipeSave
                Layout.preferredHeight: 28
                Layout.preferredWidth: 28
                antialiasing: true

                background: Rectangle {
                    color: idBtPipeSave.pressed ? Design.colButtonSelectedBackground : idBtPipeSave.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colButtonBorder
                    radius: 3
                }

                icon.source: "qrc:///img/icons/fileSave.png"
                icon.color: "transparent"
                icon.height: 22
                icon.width: 22
                padding: 1

                onPressed: {
                    idFdExamExport.visible = true;
                }

                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Sauvegarder l'examen en cours.")
            }

            Platform.FileDialog  {
                id: idFdExamExport
                title: qsTr("Enregistrement de l'examen ...")
                nameFilters: ["Excel (*.xlsx)"]
                fileMode : Platform.FileDialog.SaveFile
                property var retExport;

                folder: "file:///" + examManager.exportFolder
                onAccepted: {
                    retExport = examPipeManager.exportExam(currentFile)
                    if( retExport !== "" ) {
                        idMsgError.visible = true
                    }
                }
            }
        }

        MessageDialog {
            id: idMsgError
            //icon: Std.StandardIcon.Critical
            title: qsTr("Erreur")
            text: qsTr("L'examen n'a pas pu être sauvegardé :\n" + idFdExamExport.retExport )
            buttons: MessageDialog.Ok
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Button {
            id: idBtPipeAcq
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true
            property var keyPressed : false

            background: Rectangle {
                color: idBtPipeAcq.pressed || idBtPipeAcq.keyPressed ? Design.colButtonSelectedBackground : idBtPipeAcq.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconThicknessMm.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onPressed: {
                keyAction()
            }

            Action {
                shortcut: "v"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipeAcq.keyAction();
                idBtPipeAcq.keyPressed = true;
                timKeyPressedAcq.start();}
            }

            Action {
                shortcut: "V"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipeAcq.keyAction();
                idBtPipeAcq.keyPressed = true;
                timKeyPressedAcq.start();}
            }

            function keyAction() {
                examPipeManager.takeMeasure();
            }

            Timer {
                id: timKeyPressedAcq
                interval: 500; running: false; repeat: false
                onTriggered: idBtPipeAcq.keyPressed = false;
            }

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Saisir la [V]aleur d'épaisseur du capteur sélectionné.")
        }

        Button {
            id: idBtPipePicture
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true
            property var keyPressed : false

            background: Rectangle {
                color: idBtPipePicture.pressed || idBtPipePicture.keyPressed ? Design.colButtonSelectedBackground : idBtPipePicture.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconCamera.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onPressed: {
                keyAction()
            }

            Action {
                shortcut: "c"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipePicture.keyAction();
                idBtPipePicture.keyPressed = true;
                timKeyPressed.start();}
            }

            Action {
                shortcut: "C"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipePicture.keyAction();
                idBtPipePicture.keyPressed = true;
                timKeyPressed.start();}
            }

            function keyAction() {
                examPipeManager.takePicture();
            }

            Timer {
                id: timKeyPressed
                interval: 500; running: false; repeat: false
                onTriggered: idBtPipePicture.keyPressed = false;
            }

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("[C]apturer une photo du capteur sélectionné.")
        }

        Button {
            id: idBtPipeNext
            Layout.preferredHeight: 30
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true
            property var keyPressed : false

            background: Rectangle {
                color: (idBtPipeNext.pressed || idBtPipeNext.keyPressed) ? Design.colButtonSelectedBackground : idBtPipeNext.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconNextAcq.png"
            icon.color: "transparent"
            icon.height: 25
            icon.width: 25
            padding: 1

            onPressed: {
                keyAction()
            }

            Action {
                shortcut: "n"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipeNext.keyAction();
                idBtPipeNext.keyPressed = true;
                timKeyPressedNext.start();}
            }

            Action {
                shortcut: "N"
                enabled: examManager.controlType === 2
                onTriggered: { idBtPipeNext.keyAction();
                idBtPipeNext.keyPressed = true;
                timKeyPressedNext.start();}
            }

            function keyAction() {
                examPipeManager.nextAcquisition();
            }

            Timer {
                id: timKeyPressedNext
                interval: 500; running: false; repeat: false
                onTriggered: idBtPipeNext.keyPressed = false;
            }

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Passer à l'acquisition suivante [N].")
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Button {
            id: idBtPipeLight
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true

            checked: currentVector.funcLight().paramLLightEnable
            checkable: true

            background: Rectangle {
                color: (idBtPipeLight.pressed) ? Design.colButtonSelectedBackground : (idBtPipeLight.checked) ?  "#909000" : idBtPipeLight.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconLight.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onReleased: {
                if( !currentVector.funcLight().paramLLightEnable ) {
                    currentVector.funcLight().paramLLightLevel = 100;
                    currentVector.funcLight().paramLLightEnable = true;
                } else {
                    currentVector.funcLight().paramLLightEnable = false;
                }
            }

            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Commander la lumière des caméras.")
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }
}
