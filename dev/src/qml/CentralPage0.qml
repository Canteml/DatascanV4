import QtQuick
import Qt.labs.platform 1.1 as Platform
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0
import "rp_components"


Rectangle {
    Layout.fillWidth: true
    Layout.fillHeight : true


    Dialog {
        id: userLoginWindow
        title: qsTr("Onglets incomplete")
        modal: true
        anchors.centerIn: parent
        width: 380

        background: Rectangle {
            color: "white"
            border.width: 1
            border.color: "#4B5563"
        }

        header: Label {
            text: userLoginWindow.title
            elide: Label.ElideRight
            font.bold: true
            padding: 12
            color: "#FFFFFF"

            background: Rectangle {
                color: "#0D2B38"
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Text {
                font.family: "Roboto Condensed"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("User Name")
            }

            TextField {
                text: ""
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.preferredWidth: 220
                Layout.preferredHeight: 30
                onTextChanged: {examManager.inputUserName = text}
                Keys.onReturnPressed: {
                    console.log("Captured !")
                }
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

            }

            Text {
                font.family: "Roboto Condensed"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("Mot de passe")
            }

            TextField {
                text: ""
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.preferredWidth: 220
                Layout.preferredHeight: 30
                onTextChanged: {examManager.inputUSerPassword = text}
                Keys.onReturnPressed: {
                    console.log("Captured !")
                }
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }
            }
            RowLayout{
                Button {
                    id: btConfirmerUser
                    width: 80
                    height: 40

                    contentItem: Text {
                        text: qsTr("Confirmer")
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

                    onClicked: {
                        examManager.inputUserControl()
                        if(examManager.inputValidCheck){
                            examManager.freeSession = true
                            userLoginWindow.close();
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: btAnnulerUser
                    width: 80
                    height: 40

                    contentItem: Text {
                        text: qsTr("Annuler")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        color: Design.colMainInk
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: "white"
                        border.color: Design.colBlackBorder
                        border.width: 3
                    }

                    onClicked: {
                        userLoginWindow.close();
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5


        Item {
            Layout.fillWidth: true
            height: 30
        }


        RowLayout{
            Layout.fillWidth: true
            spacing: 5

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
                            text: qsTr("CHARGER UN PLAN")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 12
                            font.bold: Design.colFrameBorder
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
                    height: 90
                    border.width: 1
                    border.color: Design.colFrameBorder

                    Button {
                        id: btChargePlanner
                        width: 180
                        height: 60
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 15
                        anchors.leftMargin: 15

                        contentItem: Text {
                            text: qsTr("Charger un fichier")
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

                        onClicked: {
                            fileDialogChargePlannerExport.open();
                        }
                    }

                    Platform.FileDialog {
                        id: fileDialogChargePlannerExport
                        title: "Selectionnez une acquisition"
                        folder: "file:///C:/Altiscan/"
                        nameFilters: ["Fichier TOML (*.toml)"]
                        onAccepted: examManager.importPlannerToml(fileDialogChargePlannerExport.file.toString())
                    }

                    // -- Import messages (errors / warnings from ImportToml), one line each
                    Column {
                        anchors.top: btChargePlanner.top
                        anchors.left: btChargePlanner.right
                        anchors.right: parent.right
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15
                        spacing: 4

                        Repeater {
                            model: examManager.importMessages

                            delegate: Rectangle {
                                readonly property color levelColor: modelData.level === "error"   ? "#C62828"
                                                                  : modelData.level === "warning" ? "#E07B00"
                                                                  :                                 "#2E7D32"
                                width: parent.width
                                height: 60
                                color: Qt.rgba(levelColor.r, levelColor.g, levelColor.b, 0.1)
                                border.width: 1
                                border.color: levelColor

                                Text {
                                    id: idMsgText
                                    anchors.fill: parent
                                    anchors.leftMargin: 6
                                    anchors.rightMargin: 6
                                    verticalAlignment: Text.AlignVCenter
                                    text: (modelData.level === "error"   ? "✖ "
                                         : modelData.level === "warning" ? "⚠ "
                                         :                                 "✔ ") + modelData.text
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 14
                                    color: parent.levelColor
                                    wrapMode: Text.WrapAnywhere
                                }
                            }
                        }
                    }
                }
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
                            text: qsTr("SESSION LIBRE")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 12
                            font.bold: Design.colFrameBorder
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
                    height: 90
                    border.width: 1
                    border.color: Design.colFrameBorder


                    RowLayout{
                        spacing: 20
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 15
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15

                        Button {
                            Layout.preferredWidth: 180
                            Layout.preferredHeight: 60

                            contentItem: Text {
                                text: qsTr("Session libre")
                                font.family: Fonts.robotoCondensed
                                font.pixelSize: 14
                                color: Design.colMainInk
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: "white"
                                border.color: Design.colBlackBorder
                                border.width: 3
                            }

                            onClicked: {
                                examManager.freeSession = false
                                userLoginWindow.open();
                            }
                        }
                        Text {
                            visible: examManager.inputValidCheck
                            text: qsTr("Identifiant utilisateur : ")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            font.bold: true
                            color: Design.colMainInk
                            Layout.fillWidth: true
                            Layout.leftMargin: 5
                        }
                        Text {
                            visible: examManager.inputValidCheck
                            text: examManager.inputUserName
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: Design.colMainInk
                            Layout.fillWidth: true
                            Layout.leftMargin: 5
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
                        text: qsTr("CARACTÉRISTIQUES DE LÉQUIPEMENT")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 12
                        font.bold: Design.colFrameBorder
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
                height: 200
                border.width: 1
                border.color: Design.colFrameBorder

                GridLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 15
                    columns: 4
                    columnSpacing: 15
                    rowSpacing: 20

                    // -- Ligne 0 : Nom de la structure / Dossier exports

                    Text {
                        text: qsTr("Nom de la structure")
                        font.pixelSize: 14
                        font.family: Fonts.robotoCondensed
                        Layout.row: 0
                        Layout.column: 0
                    }

                    TextField {
                        text: examManager.structName
                        font.family: Fonts.robotoCondensed
                        selectByMouse: true
                        color: "black"
                        Layout.row: 0
                        Layout.column: 1
                        Layout.fillWidth: true
                        Layout.preferredWidth: 220
                        Layout.preferredHeight: 30
                        onTextChanged: {examManager.structName = text}
                        Keys.onReturnPressed: {
                            console.log("Captured !")
                        }
                        background: Rectangle {
                            color: "white"
                            border.width: 2
                            border.color: Design.colFrameBorder
                        }
                    }

                    Text {
                        text: qsTr("Dossier exports")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        Layout.row: 0
                        Layout.column: 2
                    }

                    RowLayout {
                        Layout.row: 0
                        Layout.column: 3
                        Layout.fillWidth: true
                        spacing: 5

                        TextField {
                            text: examManager.exportFolder
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            readOnly: true
                            color: "black"
                            Layout.fillWidth: true
                            Layout.preferredWidth: 185
                            Layout.preferredHeight: 30
                            background: Rectangle {
                                color: "white"
                                border.width: 2
                                border.color: Design.colFrameBorder
                            }
                        }

                        Button {
                            Layout.preferredHeight: 30
                            Layout.preferredWidth: 30
                            contentItem: Text {
                                text: "..."
                                font.bold: true
                                font.family: Fonts.robotoCondensed
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: "#0D2B38"
                                border.color: Design.colBlackBorder
                                border.width: 2
                            }

                            onPressed: {
                                folderDialog.open()
                            }
                        }
                    }

                    Platform.FolderDialog {
                        id: folderDialog
                        currentFolder: "file:///" + examManager.exportFolder
                        onFolderChanged: {
                            examManager.exportFolder = folder.toString().replace("file:///", "")
                        }
                    }

                    // -- Ligne 1 : Site / Secteur

                    Text {
                        text: qsTr("Site")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        Layout.row: 1
                        Layout.column: 0
                    }

                    TextField {
                        text: examManager.siteName
                        font.family: Fonts.robotoCondensed
                        selectByMouse: true
                        color: "black"
                        Layout.row: 1
                        Layout.column: 1
                        Layout.fillWidth: true
                        Layout.preferredWidth: 220
                        Layout.preferredHeight: 30
                        onTextChanged: {examManager.siteName = text}
                        background: Rectangle {
                            color: "white"
                            border.width: 2
                            border.color: Design.colFrameBorder
                        }
                    }

                    Text {
                        text: qsTr("Secteur")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        Layout.row: 1
                        Layout.column: 2
                    }

                    TextField {
                        text: examManager.sectorName
                        font.family: Fonts.robotoCondensed
                        selectByMouse: true
                        color: "black"
                        Layout.row: 1
                        Layout.column: 3
                        Layout.fillWidth: true
                        Layout.preferredWidth: 220
                        Layout.preferredHeight: 30
                        onTextChanged: {examManager.sectorName = text}
                        background: Rectangle {
                            color: "white"
                            border.width: 2
                            border.color: Design.colFrameBorder
                        }
                    }

                    // -- Ligne 3 : Origine

                    Text {
                        text: qsTr("Origine")
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 14
                        Layout.row: 2
                        Layout.column: 0
                    }

                    TextField {
                        text: examManager.examOrigin
                        font.family: Fonts.robotoCondensed
                        selectByMouse: true
                        color: "black"
                        Layout.row: 2
                        Layout.column: 1
                        Layout.fillWidth: true
                        Layout.preferredWidth: 220
                        Layout.preferredHeight: 30
                        onTextChanged: {examManager.examOrigin = text}
                        background: Rectangle {
                            color: "white"
                            border.width: 2
                            border.color: Design.colFrameBorder
                        }
                    }
                }
            }
        }


        /*
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 15

            Text {
                text: qsTr("Activité :")
                font.pixelSize: 14
            }

            Switch {
                text: checked ? qsTr("En service") : qsTr("Hors service")
                Layout.leftMargin: 10
                font.pixelSize: 14
                onCheckedChanged: {
                    examManager.structureActive = checked
                }
                Component.onCompleted: {
                    checked = examManager.structureActive
                }
            }
        }

        RowLayout {
            Layout.fillWidth : true
            Layout.leftMargin: 15

            Button {
                id: idBtCtl0
                Layout.preferredHeight: 50
                text: qsTr("CONTROLE\nBSCAN")
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
                text: qsTr("CONTROLE\nMEP")
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
                visible: products.hasPipeEnabled
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

        FormSection {
            text: qsTr("Plan de contrôle")
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
                            validator: RegularExpressionValidator {
                                regularExpression: /([0-9]+(\.[0-9]+))/
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
                            validator: RegularExpressionValidator {
                                regularExpression: /([0-9]+(\.[0-9]+))/
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
                        text: qsTr("Découper en viroles :")
                        font.pixelSize: 14
                    }

                    Switch {
                        id: idEnableViroles
                        text: checked ? qsTr("Oui") : qsTr("Non")
                        Layout.leftMargin: 10
                        font.pixelSize: 14
                        onCheckedChanged: {
                            examManager.virolesEnabled = checked
                        }
                        Component.onCompleted: {
                            checked = examManager.virolesEnabled
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: examManager.virolesEnabled

                    Text {
                        text: qsTr("Nombre de mesures par virole :")
                        Layout.leftMargin: 10
                        font.pixelSize: 14
                    }

                    RpSpinBox {
                        id: idPerVirole
                        from: 1
                        to: 1000
                        value: examManager.measuresPerVirole
                        onValueChanged: {
                            examManager.measuresPerVirole = idPerVirole.value
                        }
                    }

                    Text {
                        text: qsTr("Nombre de viroles par génératrice :")
                        Layout.leftMargin: 10
                        font.pixelSize: 14
                    }

                    RpSpinBox {
                        id: idNbViroles
                        from: 1
                        to: 1000
                        Component.onCompleted: {
                            idNbViroles.value = examManager.mepNbRow / examManager.measuresPerVirole
                        }
                        onVisibleChanged: {
                            if(!idNbViroles.visible) return
                            idNbViroles.value = examManager.mepNbRow / examManager.measuresPerVirole
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: qsTr("■  Nombre de génératrices :")+" "
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
                        text: qsTr("Périmètre (m) : ")
                        //Layout.leftMargin: 10
                    }
                    TextField {
                        id: idTxtPerimeterMep
                        Layout.preferredWidth: 80
                        selectByMouse: true
                        Layout.preferredHeight: 30
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
                        text: qsTr("■  Nombre de mesures par génératrice :")
                    }
                    RpSpinBox {
                        id: idSbNbMesH
                        enabled: !examManager.virolesEnabled
                        from: 1
                        to: 1000
                        value: (!idNbViroles.visible) ? examManager.mepNbRow : examManager.measuresPerVirole * idNbViroles.value
                        onValueChanged: {
                            examManager.mepNbRow = idSbNbMesH.value
                        }
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
                        onTextEdited: {examManager.mepHeight = parseFloat(idTxtHeightMep.text)}
                        Connections {
                            target : examManager
                            function onMepHeightChanged() {
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
        }*/

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
