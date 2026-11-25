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
                text: qsTr("Profil")
            }

            ComboBox {
                id: idCbProfil
                Layout.fillWidth: true
                Layout.preferredWidth: 220
                Layout.preferredHeight: 30
                font.family: Fonts.robotoCondensed
                Layout.row: 0
                Layout.column: 1
                model: [
                    { text: "",                    value: "" },
                    { text: qsTr("OPERATEUR"),     value: "operateur" },
                    { text: qsTr("ADMINISTRATEUR"), value: "administrateur" }
                ]
                textRole: "text"
                valueRole: "value"

                background: Rectangle {
                    color: "white"
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                contentItem: Text {
                    font.family: "Roboto Condensed"
                    text: idCbProfil.displayText
                    font.pixelSize: 12
                    font.bold: true
                    leftPadding: 6
                    color: "#0D2B38"
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                popup: Popup {
                    y: idCbProfil.height
                    width: idCbProfil.width
                    implicitHeight: contentItem.implicitHeight
                    padding: 1

                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: idCbProfil.popup.visible ? idCbProfil.delegateModel : null
                        currentIndex: idCbProfil.highlightedIndex
                        ScrollIndicator.vertical: ScrollIndicator {}
                    }

                    background: Rectangle {
                        color: "#FFFFFF"
                        border.color: "#C7CBD1"
                        border.width: 1
                    }
                }
                delegate: ItemDelegate {
                    id: idCbProfilDelegate
                    width: idCbProfil.width
                    highlighted: idCbProfil.highlightedIndex === index
                    contentItem: Text {
                        font.family: "Roboto Condensed"
                        text: idCbProfil.textAt(index)
                        font.pixelSize: 12
                        font.bold: true
                        color: "#0D2B38"
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    background: Rectangle {
                        color: idCbProfilDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                    }
                }

                onActivated: function(index) {
                    examManager.inputProfil = valueAt(index)
                }
                Component.onCompleted: {
                    currentIndex = indexOfValue(examManager.inputProfil)
                }
            }

            Text {
                font.family: "Roboto Condensed"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("Nom")
            }

            TextField {
                text: ""
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.row: 1
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
                text: qsTr("Prenom")
            }

            TextField {
                text: ""
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.preferredWidth: 220
                Layout.preferredHeight: 30
                onTextChanged: {examManager.inputUserPrenom = text}
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
                text: qsTr("Numéro de carte")
            }

            TextField {
                text: ""
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.preferredWidth: 220
                Layout.preferredHeight: 30
                onTextChanged: {examManager.inputCarteNumero = text}
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
                visible: examManager.inputProfil === "administrateur"
            }

            TextField {
                id: idTfPassword
                text: ""
                visible: examManager.inputProfil === "administrateur"
                echoMode: TextInput.Password
                onVisibleChanged: { if (!visible) text = "" }
                font.family: Fonts.robotoCondensed
                selectByMouse: true
                color: "black"
                Layout.row: 2
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
            height: 10
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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
                    Layout.preferredHeight: 40
                    border.width: 1
                    border.color: Design.colFrameBorder

                    RowLayout {
                        anchors.fill: parent
                        spacing: 5

                        Text {
                            text: qsTr("CHARGER UN PROJET")
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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
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
                            text:
                                if(examManager.inputProfil === "administrateur"){
                                    return qsTr("ADMINISTRATEUR")
                                }else if(examManager.inputProfil === "operateur"){
                                    return qsTr("OPERATEUR")
                                }else{
                                    return qsTr("")
                                }
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            font.bold: true
                            color:
                                if(examManager.inputProfil === "administrateur"){
                                    return "#12603F"
                                }else{
                                    return "#E07B00"
                                }
                            Layout.fillWidth: true
                            Layout.leftMargin: 5
                        }
                        Text {
                            visible: examManager.inputValidCheck
                            text: qsTr("Identifiant utilisateur")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            font.bold: true
                            color: Design.colMainInk
                            Layout.fillWidth: true
                            Layout.leftMargin: 5
                        }
                        Text {
                            visible: examManager.inputValidCheck
                            text: examManager.inputUserPrenom + " " + examManager.inputUserName
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
            height: 10
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            Rectangle {
                color: "#D5DEE3"
                Layout.fillWidth: true
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                Layout.preferredHeight: 40
                border.width: 1
                border.color: Design.colFrameBorder

                RowLayout {
                    anchors.fill: parent
                    spacing: 5

                    Text {
                        text: qsTr("CARACTÉRISTIQUES DE L'ÉQUIPEMENT")
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
                Layout.leftMargin: 5
                Layout.rightMargin: 5
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
                        text: qsTr("Nom")
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
                                color: "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: "white"
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
