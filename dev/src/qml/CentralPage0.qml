import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Qt.labs.platform 1.1
import "rp_components"

Rectangle {
    Layout.fillWidth: true
    Layout.fillHeight : true    

    ColumnLayout {
        anchors.fill: parent        
        anchors.margins: 5

        FormSection {
            text: qsTr("Information sur l'ouvrage")
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 15

            Text {
                text: qsTr("Nom de la structure :")
                font.pixelSize: 14
            }

            TextField {
                text: examManager.structName
                selectByMouse: true
                Layout.fillWidth: true;
                Layout.preferredHeight: 30
                onTextChanged: {examManager.structName = text}

            }

            Text {
                text: qsTr("Nom de la Pièce :")
                font.pixelSize: 14
                Layout.leftMargin: 10
            }

            ComboBox {
                id: idCbPiece
                model: examManager.commonPieceNames
                Layout.preferredHeight: 30
                editable: true
                onAccepted: { examManager.pieceName = editText }
                onCurrentTextChanged: { examManager.pieceName = currentText }
                Component.onCompleted: {
                        contentItem.selectByMouse = true
                }
            }

            Text {
                text: qsTr("Dossier CSV :")
                font.pixelSize: 14
                Layout.leftMargin: 10
            }

            TextField {
                text: examManager.exportFolder
                selectByMouse: true
                readOnly: true
                Layout.fillWidth: true;
                Layout.preferredHeight: 30
            }

            Button {
                text: "..."
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                onPressed: {
                    folderDialog.open()
                }
            }

            FolderDialog {
                id: folderDialog
                currentFolder: "file:///" + examManager.exportFolder
                onFolderChanged: {
                    examManager.exportFolder = folder.toString().replace("file:///", "")
                }
            }
        }

        FormSection {
            text: qsTr("Plan de contrôle")
        }

        RowLayout {
            Layout.fillWidth : true
            Layout.leftMargin: 15

            Button {
                id: idBtCtl0
                Layout.preferredHeight: 50
                text: qsTr("GENERATRICE\nBSCAN")
                checked: examManager.controlType === 0
                onClicked: {examManager.controlType = 0}
                onCheckedChanged: {checked = (examManager.controlType === 0)}
                Connections {
                    target: examManager
                    function onControlTypeChanged() {idBtCtl0.checked = (examManager.controlType === 0)}
                }
            }
            Button {
                id: idBtCtl1
                Layout.preferredHeight: 50
                text: qsTr("GENERATRICE\nMEP")
                checked: examManager.controlType === 1
                onClicked: {examManager.controlType = 1}
                onCheckedChanged: {checked = (examManager.controlType === 1)}
                Connections {
                    target: examManager
                    function onControlTypeChanged() {idBtCtl1.checked = (examManager.controlType === 1)}
                }
            }
            Button {
                id: idBtCtl2
                Layout.preferredHeight: 50
                text: qsTr("TUYAU\n4 POINTS")
                checked: examManager.controlType === 2
                onClicked: {examManager.controlType = 2}
                onCheckedChanged: {checked = (examManager.controlType === 2)}
                Connections {
                    target: examManager
                    function onControlTypeChanged() {idBtCtl2.checked = (examManager.controlType === 2)}
                }
            }
        }

        StackLayout {
            id: idSlCtl
            currentIndex: examManager.controlType
            Layout.fillWidth: true
            Layout.leftMargin: 15

            // -- PAGE GENERATRICE BSCAN

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: qsTr("■  Nombre de génératrices :")
                    }
                    RpSpinBox {
                        id: idSbNbGene
                        from: 1
                        to: 1000
                        value: examManager.bscanNbGeneratrix
                        onValueChanged: { examManager.bscanNbGeneratrix = idSbNbGene.value; }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 40
                    Text {
                        text: qsTr("Intervalle (m) : ")
                    }
                    Text {
                        text: examManager.bscanInterval.toFixed(2)
                        font.bold: true
                        Layout.minimumWidth: 40
                    }

                    GridLayout {
                        columns: 4

                        Text {
                            text: qsTr("Périmètre (m) : ")
                            Layout.leftMargin: 10
                        }
                        TextField {
                            id: idTxtPerimeter
                            Layout.preferredWidth: 80
                            selectByMouse: true
                            Layout.preferredHeight: 30
                            validator: RegExpValidator {
                                regExp: /([0-9]+(\.[0-9]+))/
                            }
                            onTextEdited: {examManager.bscanPerimeter = parseFloat(idTxtPerimeter.text)}
                            Connections {
                                target : examManager
                                function onBscanPerimeterChanged() {
                                    if(!idTxtPerimeter.focus) {
                                        idTxtPerimeter.text = examManager.bscanPerimeter.toFixed(2)
                                    }
                                }
                            }
                            Component.onCompleted: {
                                idTxtPerimeter.text = examManager.bscanPerimeter.toFixed(2)
                            }
                        }

                        Text {
                            text: qsTr("Diametre (m) : ")
                            Layout.leftMargin: 10
                        }
                        TextField {
                            id: idTxtDiameter
                            Layout.preferredWidth: 80
                            selectByMouse: true
                            Layout.preferredHeight: 30
                            validator: RegExpValidator {
                                regExp: /([0-9]+(\.[0-9]+))/
                            }
                            onTextEdited: {examManager.bscanDiameter = parseFloat(idTxtDiameter.text)}
                            Connections {
                                target : examManager
                                function onBscanDiameterChanged() {
                                    if(!idTxtDiameter.focus) {
                                        idTxtDiameter.text = examManager.bscanDiameter.toFixed(2)
                                    }
                                }
                            }
                            Component.onCompleted: {
                                idTxtDiameter.text = examManager.bscanDiameter.toFixed(2)
                            }
                        }
                    }
                }
            }

            // -- PAGE GENERATRICE MEP

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: qsTr("■  Nombre de mesures sur la largeur :")+" "
                    }
                    RpSpinBox {
                        id: idSbNbMesW
                        from: 1
                        to: 1000
                        value: examManager.mepNbCol
                        onValueChanged: { examManager.mepNbCol = idSbNbMesW.value; }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 40
                    Text {
                        text: qsTr("Intervalle X (m) : ")
                        Layout.minimumWidth: 105
                    }
                    Text {
                        text: examManager.mepIntervalX.toFixed(2)
                        font.bold: true
                        Layout.minimumWidth: 40
                    }

                    Text {
                        text: qsTr("Périmètre (m) : ")
                        Layout.leftMargin: 10
                    }
                    TextField {
                        id: idTxtPerimeterMep
                        Layout.preferredWidth: 80
                        selectByMouse: true
                        Layout.preferredHeight: 30
                        validator: RegExpValidator {
                            regExp: /([0-9]+(\.[0-9]+))/
                        }
                        onTextEdited: {examManager.mepPerimeter = parseFloat(idTxtPerimeterMep.text)}
                        Connections {
                            target : examManager
                            function onMepPerimeterChanged() {
                                if(!idTxtPerimeterMep.focus) {
                                    idTxtPerimeterMep.text = examManager.mepPerimeter.toFixed(2)
                                }
                            }
                        }
                        Component.onCompleted: {
                            idTxtPerimeterMep.text = examManager.mepPerimeter.toFixed(2)
                        }
                    }

                    Text {
                        text: qsTr("Diamètre (m) : ")
                        Layout.leftMargin: 10
                    }
                    TextField {
                        id: idTxtDiameterMep
                        Layout.preferredWidth: 80
                        selectByMouse: true
                        Layout.preferredHeight: 30
                        validator: RegExpValidator {
                            regExp: /([0-9]+(\.[0-9]+))/
                        }
                        onTextEdited: {examManager.mepDiameter = parseFloat(idTxtDiameterMep.text)}
                        Connections {
                            target : examManager
                            function onMepDiameterChanged() {
                                if(!idTxtDiameterMep.focus) {
                                    idTxtDiameterMep.text = examManager.mepDiameter.toFixed(2)
                                }
                            }
                        }
                        Component.onCompleted: {
                            idTxtDiameterMep.text = examManager.mepDiameter.toFixed(2)
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: qsTr("■  Nombre de mesures sur la hauteur :")
                    }
                    RpSpinBox {
                        id: idSbNbMesH
                        from: 1
                        to: 1000
                        value: examManager.mepNbRow
                        onValueChanged: { examManager.mepNbRow = idSbNbMesH.value; }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 40
                    Text {
                        text: qsTr("Intervalle Y (m) : ")
                        Layout.minimumWidth: 105
                    }
                    Text {
                        text: examManager.mepIntervalY.toFixed(2)
                        font.bold: true
                        Layout.minimumWidth: 40
                    }

                    Text {
                        text: qsTr("Hauteur (m) : ")
                        Layout.leftMargin: 10
                    }
                    TextField {
                        id: idTxtHeightMep
                        Layout.preferredWidth: 80
                        selectByMouse: true
                        Layout.preferredHeight: 30
                        validator: RegExpValidator {
                            regExp: /([0-9]+(\.[0-9]+))/
                        }
                        onTextEdited: {examManager.mepHeight = parseFloat(idTxtHeightMep.text)}
                        Connections {
                            target : examManager
                            function onMepPerimeterChanged() {
                                if(!idTxtHeightMep.focus) {
                                    idTxtHeightMep.text = examManager.mepHeight.toFixed(2)
                                }
                            }
                        }
                        Component.onCompleted: {
                            idTxtHeightMep.text = examManager.mepHeight.toFixed(2)
                        }
                    }
                }
            }
        }

        Item {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.visible: false
            idBtNext.text : qsTr("PREPARER")

            Connections {
                target: idBannerBottom2.idBtNext
                function onClicked() {
                    globalManager.guiPage = 1;
                }
            }
        }
    }
}
