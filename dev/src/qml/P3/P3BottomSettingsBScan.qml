import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    id: idRectBottomSettingsBScan

    // Shared state with P3SettingsBar / P3Top, set externally
    property var slCamSettings: null

    // Partie affichée : "bscan" (idLeftHalf) ou "codeur" (idRightHalf)
    property string openingMode: "bscan"

    color: "white"
    border.width: 1
    border.color: Design.colFrameBorder

    Popup {
        id: idSavePresetPopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 320
        modal: true
        focus: true
        padding: 0
        bottomPadding: 15

        property bool confirmOverwrite: false

        onOpened: {
            confirmOverwrite = false;
            idPresetNameField.text = "";
            idPresetNameField.forceActiveFocus();
        }

        function doSave() {
            var name = idPresetNameField.text.trim();
            if(name.length === 0) return;
            if(!confirmOverwrite && scanManager.bscanPresetExists(name)){
                confirmOverwrite = true;
                return;
            }
            if(scanManager.bscanSavePreset(name)){
                scanManager.bscanSaveUserParams();
                close();
            }
        }

        background: Rectangle {
           color: "white"
           border.width: 2
           border.color: Design.colFrameBorder
        }

        contentItem: ColumnLayout {
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                color: Design.colButtonDarkBackground

                Text {
                    text: qsTr("NOM DE LA CONFIGURATION")
                    font.family: Fonts.robotoCondensed
                    font.pixelSize: 12
                    font.bold: true
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            TextField {
                id: idPresetNameField
                Layout.topMargin: 10
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                Layout.fillWidth: true
                Layout.preferredHeight: 25
                font.family: Fonts.robotoCondensed
                maximumLength: 40
                selectByMouse: true
                onTextChanged: idSavePresetPopup.confirmOverwrite = false
                onAccepted: idSavePresetPopup.doSave()
            }

            Text {
                visible: idSavePresetPopup.confirmOverwrite
                text: qsTr("Cette configuration existe déjà. Appuyer à nouveau pour l'écraser.")
                color: "#C62828"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.topMargin: 10
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                font.family: Fonts.robotoCondensed
                font.pixelSize: 10
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 35
                Layout.topMargin: 10
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                Button {
                    Layout.preferredWidth: (parent.width - 5) / 2
                    Layout.fillHeight: true

                    background: Rectangle {
                        color: "white"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("ANNULER")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 10
                        font.bold: true
                        color: Design.colButtonDarkBackground
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: idSavePresetPopup.close()
                }
                Button {
                    Layout.leftMargin: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: idSavePresetPopup.confirmOverwrite ? qsTr("ÉCRASER") : qsTr("ENREGISTRER")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 10
                        font.bold: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    enabled: idPresetNameField.text.trim().length > 0
                    onClicked: idSavePresetPopup.doSave()
                }
            }
        }
    }

    Popup {
        id: idLoadPresetPopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 320
        height: 360
        modal: true
        focus: true
        // Padding = épaisseur de la bordure : le bandeau titre occupe toute la largeur
        padding: 2
        bottomPadding: 15

        property var presetNames: []

        onOpened: presetNames = scanManager.bscanListPresets()

        background: Rectangle {
            color: "white"
            border.width: 2
            border.color: Design.colFrameBorder
        }

        contentItem: ColumnLayout {
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                color: Design.colButtonDarkBackground

                Text {
                    text: qsTr("CHARGER UNE CONFIGURATION")
                    font.family: Fonts.robotoCondensed
                    font.pixelSize: 12
                    font.bold: true
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            ListView{
                id: idPresetList
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 15
                Layout.rightMargin: 15
                Layout.topMargin: 10
                clip: true
                spacing: 4
                model: idLoadPresetPopup.presetNames
                ScrollBar.vertical: ScrollBar {
                }

                delegate: Button {
                    width: idPresetList.width
                    height: 32

                    background: Rectangle {
                        color: parent.down ? "#16253A" : "white"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: modelData
                        elide: Text.ElideRight
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 11
                        font.bold: true
                        color: parent.down ? "white" : Design.colButtonDarkBackground
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        leftPadding: 8
                    }

                    onClicked: {
                        if( scanManager.bscanLoadPreset(modelData) ) idLoadPresetPopup.close();
                    }
                }

                // Aucune configuration sauvegardée
                Text {
                    anchors.centerIn: parent
                    visible: idPresetList.count === 0
                    text: qsTr("Aucune configuration enregistrée")
                    font.family: Fonts.robotoCondensed
                    font.pixelSize: 10
                    color: "#4B5563"
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.leftMargin: 15
                Layout.rightMargin: 15
                Layout.preferredHeight: 30


                background: Rectangle {
                    color: "white"
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("ANNULER")
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 10
                    font.bold: true
                    color: Design.colButtonDarkBackground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: idLoadPresetPopup.close()
            }
        }
    }

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


        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            Rectangle {
                id: rectangleMenuBScanOption
                Layout.preferredWidth: 40
                Layout.fillHeight: true
                color: "#EEF0F3"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 3
                    spacing: 3

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: idRectBottomSettingsBScan.openingMode === "bscan" ? "#16253A" : "transparent"

                        Text {
                            font.family: "Roboto Condensed"
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("B-SCAN")
                            font.pixelSize: 13
                            font.bold: idRectBottomSettingsBScan.openingMode === "bscan"
                            color: idRectBottomSettingsBScan.openingMode === "bscan" ? "#FFFFFF" : "#4B5563"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: idRectBottomSettingsBScan.openingMode = "bscan"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: idRectBottomSettingsBScan.openingMode === "codeur" ? "#16253A" : "transparent"

                        Text {
                            font.family: "Roboto Condensed"
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("CODEUR")
                            font.pixelSize: 13
                            font.bold: idRectBottomSettingsBScan.openingMode === "codeur"
                            color: idRectBottomSettingsBScan.openingMode === "codeur" ? "#FFFFFF" : "#4B5563"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: idRectBottomSettingsBScan.openingMode = "codeur"
                        }
                    }
                }
            }

            Item {
                id: idColumnsContainer
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 10

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        id: idLeftHalf
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: idRectBottomSettingsBScan.openingMode === "bscan"
                        clip: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 8

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignTop
                                spacing: 0

                                Item {Layout.fillWidth: true }

                                // COLONNE 1 - centrée à 1/4 de la largeur
                                ColumnLayout{
                                    id: idColumn1
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    TextMetrics {
                                        id: idTmColumn1Value
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        text: "10 m"
                                    }

                                    // GAIN SETTINGS
                                    // COLUMN 1 - RAW 1
                                    Text {
                                        text: qsTr("RÉSOLUTION X")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtResolutionMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color: "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                for (let i = 0; i < 5; i++) {
                                                    scanManager.ascanZoomOut()
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtResolutionMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color: "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                scanManager.ascanZoomOut()
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtResolutionPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color: "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                scanManager.ascanZoomIn()
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtResolutionPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color: "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                for (let i = 0; i < 5; i++) {
                                                    scanManager.ascanZoomIn()
                                                }
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("DISTANCE AFFICHÉE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtDistanceMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeXScaleM > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDistanceMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeXScaleM - 1 < 1){
                                                    scanManager.BScanAxeXScaleM = 1
                                                }else {
                                                    scanManager.BScanAxeXScaleM = scanManager.BScanAxeXScaleM - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idDistanceValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn1Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idDistanceValue.editing
                                                text: scanManager.BScanAxeXScaleM + " m"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idDistanceValue.editing
                                                onDoubleClicked: {
                                                    idDistanceInput.text = scanManager.BScanAxeXScaleM
                                                    idDistanceValue.editing = true
                                                    idDistanceInput.forceActiveFocus()
                                                    idDistanceInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idDistanceValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idDistanceInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idDistanceInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 2
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1; top: 10 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.BScanAxeXScaleM = parseInt(text)
                                                        idDistanceValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idDistanceValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idDistanceValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtDistancePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeXScaleM < 10

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDistancePlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeXScaleM + 1 > 10){
                                                    scanManager.BScanAxeXScaleM = 10
                                                }else {
                                                    scanManager.BScanAxeXScaleM = scanManager.BScanAxeXScaleM + 1
                                                }
                                            }
                                        }
                                    }

                                    // DEBUT PORTE ROUGE 2
                                    // COLUMN 1 - RAW 3
                                    Text {
                                        text: qsTr("AXE HORIZONTAL")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Button {
                                        id: idBtAxeTemps
                                        Layout.preferredWidth: 165
                                        Layout.preferredHeight: 25

                                        background: Rectangle {
                                            color: scanManager.BScanAxeXType === 1 ? Design.colButtonBackground : "white"
                                            border.width: 2
                                            border.color: Design.colFrameBorder
                                        }

                                        contentItem: Text {
                                            text: scanManager.BScanAxeXType === 1 ? qsTr("TEMPS") : qsTr("DISTANCE")
                                            font.family: Fonts.robotoCondensed
                                            font.pointSize: 12
                                            font.bold: true
                                            color: scanManager.BScanAxeXType === 1 ? "white" : Design.colButtonDarkBackground
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: scanManager.BScanAxeXType = 1 - scanManager.BScanAxeXType
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Rectangle {
                                    Layout.topMargin: 5
                                    Layout.bottomMargin: 5
                                    Layout.preferredWidth: 1
                                    Layout.fillHeight: true
                                    color : Design.colSeparator
                                }

                                Item { Layout.fillWidth: true }

                                // COLONNE 2 - centrée à 1/2 de la largeur
                                ColumnLayout{
                                    id: idColumn2
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    TextMetrics {
                                        id: idTmColumn2Value
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        text: "200 mm"
                                    }

                                    Text {
                                        text: qsTr("ECHELLE Y MIN")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtEchelleMinMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMinMm > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMinMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMinMm - 10 < 0){
                                                    scanManager.BScanAxeYMinMm = 0
                                                }else{
                                                    scanManager.BScanAxeYMinMm = scanManager.BScanAxeYMinMm - 10
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMinMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMinMm > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMinMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMinMm - 1 < 0){
                                                    scanManager.BScanAxeYMinMm = 0
                                                }else{
                                                    scanManager.BScanAxeYMinMm = scanManager.BScanAxeYMinMm - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idEchelleMinValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idEchelleMinValue.editing
                                                text: scanManager.BScanAxeYMinMm + " mm"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idEchelleMinValue.editing
                                                onDoubleClicked: {
                                                    idEchelleMinInput.text = scanManager.BScanAxeYMinMm
                                                    idEchelleMinValue.editing = true
                                                    idEchelleMinInput.forceActiveFocus()
                                                    idEchelleMinInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idEchelleMinValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idEchelleMinInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idEchelleMinInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 3
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 0; top: (scanManager.BScanAxeYMaxMm - 1) }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.BScanAxeYMinMm = parseInt(text)
                                                        idEchelleMinValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idEchelleMinValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idEchelleMinValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMinPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMinMm < (scanManager.BScanAxeYMaxMm - 1)

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMinPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMinMm + 1 > (scanManager.BScanAxeYMaxMm - 1)){
                                                    scanManager.BScanAxeYMinMm = (scanManager.BScanAxeYMaxMm - 1)
                                                }else{
                                                    scanManager.BScanAxeYMinMm = scanManager.BScanAxeYMinMm + 1
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMinPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMinMm < (scanManager.BScanAxeYMaxMm - 1)

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMinPlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMinMm + 10 > (scanManager.BScanAxeYMaxMm - 1)){
                                                    scanManager.BScanAxeYMinMm = (scanManager.BScanAxeYMaxMm - 1)
                                                }else{
                                                    scanManager.BScanAxeYMinMm = scanManager.BScanAxeYMinMm + 10
                                                }
                                            }
                                        }
                                    }


                                    Text {
                                        text: qsTr("ECHELLE Y MAX")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtEchelleMaxMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMaxMm > scanManager.BScanAxeYMinMm + 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMaxMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMaxMm - 10 < (scanManager.BScanAxeYMinMm + 1)){
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMinMm + 1
                                                }else{
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMaxMm - 10
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMaxMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMaxMm > scanManager.BScanAxeYMinMm + 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMaxMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMaxMm - 1 < (scanManager.BScanAxeYMinMm + 1)){
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMinMm + 1
                                                }else{
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMaxMm - 1
                                                }
                                            }
                                        }


                                        Item {
                                            id: idEchelleMaxValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idEchelleMaxValue.editing
                                                text: scanManager.BScanAxeYMaxMm + " mm"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idEchelleMaxValue.editing
                                                onDoubleClicked: {
                                                    idEchelleMaxInput.text = scanManager.BScanAxeYMaxMm
                                                    idEchelleMaxValue.editing = true
                                                    idEchelleMaxInput.forceActiveFocus()
                                                    idEchelleMaxInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idEchelleMaxValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idEchelleMaxInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idEchelleMaxInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 3
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: (scanManager.BScanAxeYMinMm + 1); top: 200 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.BScanAxeYMaxMm = parseInt(text)
                                                        idEchelleMaxValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idEchelleMaxValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idEchelleMaxValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMaxPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMaxMm < 200

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMaxPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMaxMm + 1 > 200){
                                                    scanManager.BScanAxeYMaxMm = 200
                                                }else{
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMaxMm + 1
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtEchelleMaxPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.BScanAxeYMaxMm < 200

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtEchelleMaxPlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(scanManager.BScanAxeYMaxMm + 10 > 200){
                                                    scanManager.BScanAxeYMaxMm = 200
                                                }else{
                                                    scanManager.BScanAxeYMaxMm = scanManager.BScanAxeYMaxMm + 10
                                                }
                                            }
                                        }
                                    }


                                    // SEUIL DE BRUIT
                                    // COLUMN 2 - RAW 1
                                    Text {
                                        text: qsTr("ZONE AUTO DÉTECTION")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtAutoDetectionMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: examManager.zoneAutoHoleSizeMm > 10

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtAutoDetectionMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(examManager.zoneAutoHoleSizeMm - 20 < 10){
                                                    examManager.zoneAutoHoleSizeMm = 10
                                                }
                                                else
                                                    examManager.zoneAutoHoleSizeMm = examManager.zoneAutoHoleSizeMm - 20
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtAutoDetectionMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: examManager.zoneAutoHoleSizeMm > 10

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtAutoDetectionMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(examManager.zoneAutoHoleSizeMm - 5 < 10){
                                                    examManager.zoneAutoHoleSizeMm = 10
                                                }
                                                else
                                                    examManager.zoneAutoHoleSizeMm = examManager.zoneAutoHoleSizeMm - 5
                                            }
                                        }

                                        Text {
                                            text: examManager.zoneAutoHoleSizeMm
                                            font.family: Fonts.robotoCondensed
                                            font.pixelSize: 10
                                            font.bold: true
                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            horizontalAlignment: Text.AlignHCenter
                                        }

                                        P3RepeatButton {
                                            id: idBtAutoDetectionPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: examManager.zoneAutoHoleSizeMm < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtAutoDetectionPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(examManager.zoneAutoHoleSizeMm + 5 > 100){
                                                    examManager.zoneAutoHoleSizeMm = 100
                                                }
                                                else
                                                    examManager.zoneAutoHoleSizeMm = examManager.zoneAutoHoleSizeMm + 5
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtAutoDetectionPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: examManager.zoneAutoHoleSizeMm < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtAutoDetectionPlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(examManager.zoneAutoHoleSizeMm + 20 > 100){
                                                    examManager.zoneAutoHoleSizeMm = 100
                                                }
                                                else
                                                    examManager.zoneAutoHoleSizeMm = examManager.zoneAutoHoleSizeMm + 20
                                            }
                                        }
                                    }
                                }
                                Item { Layout.fillWidth: true }
                            }
                        }
                    }

                    Item {
                        id: idRightHalf
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: idRectBottomSettingsBScan.openingMode === "codeur"
                        clip: true

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

                                TextMetrics {
                                    id: idBcColumn1Value
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 10
                                    font.bold: true
                                    text: "1000.00 mm"
                                }

                                ColumnLayout {
                                    id: idRhightColumn1
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8


                                    // Debut POrte 1 Setting
                                    // COLUMN 1 - RAW 2
                                    Text {
                                        text: qsTr("DIAMÈTRE ROUE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtDiametreMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.wheelDiameterMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDiametreMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.wheelDiameterMm = encoderController.wheelDiameterMm - 1
                                        }

                                        P3RepeatButton {
                                            id: idBtDiametreMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.wheelDiameterMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDiametreMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.wheelDiameterMm = encoderController.wheelDiameterMm - 0.1
                                        }

                                        Item {
                                            id: idDiametreValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idBcColumn1Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idDiametreValue.editing
                                                text: encoderController.wheelDiameterMm.toFixed(2) + " mm"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idDiametreValue.editing
                                                onDoubleClicked: {
                                                    idDiametreInput.text = encoderController.wheelDiameterMm.toFixed(2)
                                                    idDiametreValue.editing = true
                                                    idDiametreInput.forceActiveFocus()
                                                    idDiametreInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idDiametreValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idDiametreInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idDiametreInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 7    // "1000.00"
                                                    selectByMouse: true
                                                    // up to 2 decimals, "." or "," ; "56." is Intermediate so it can still be typed
                                                    validator: RegularExpressionValidator { regularExpression: /^\d{1,4}([.,]\d{1,2})?$/ }

                                                    readonly property real value: parseFloat(text.replace(",", "."))
                                                    readonly property bool valid: acceptableInput && value >= 1 && value <= 1000

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        if(!valid)
                                                            return

                                                        encoderController.wheelDiameterMm = Math.round(value * 100) / 100
                                                        idDiametreValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idDiametreValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idDiametreValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtDiametrePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.wheelDiameterMm < 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDiametrePlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.wheelDiameterMm = encoderController.wheelDiameterMm + 0.1
                                        }

                                        P3RepeatButton {
                                            id: idBtDiametrePlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.wheelDiameterMm < 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtDiametrePlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.wheelDiameterMm = encoderController.wheelDiameterMm + 1
                                        }
                                    }

                                    // LARGEUR PORTE 1
                                    // COLUMN 2 - RAW 2
                                    Text {
                                        text: qsTr("PAS CODEUR")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtPasCodeurMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.coderResolution > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtPasCodeurMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.coderResolution = encoderController.coderResolution - 100
                                        }

                                        P3RepeatButton {
                                            id: idBtPasCodeurMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.coderResolution > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtPasCodeurMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.coderResolution = encoderController.coderResolution - 1
                                        }


                                        Item {
                                            id: idDPasCodeurValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idBcColumn1Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idDPasCodeurValue.editing
                                                text: encoderController.coderResolution
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idDPasCodeurValue.editing
                                                onDoubleClicked: {
                                                    idPasCodeurInput.text = encoderController.coderResolution
                                                    idDPasCodeurValue.editing = true
                                                    idPasCodeurInput.forceActiveFocus()
                                                    idPasCodeurInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idDPasCodeurValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idPasCodeurInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idPasCodeurInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 6
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1; top: 200000 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        encoderController.coderResolution = parseInt(text)
                                                        idDPasCodeurValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idDPasCodeurValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idDPasCodeurValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtPasCodeurPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.coderResolution < 200000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtPasCodeurPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.coderResolution = encoderController.coderResolution + 1
                                        }

                                        P3RepeatButton {
                                            id: idBtPasCodeurPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.coderResolution < 200000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtPasCodeurPlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: encoderController.coderResolution = encoderController.coderResolution + 100
                                        }
                                    }

                                    Text {
                                        text: qsTr("SENS DU CODEUR")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Button {
                                        id: idBtSenseCodeur
                                        Layout.preferredWidth: 212
                                        Layout.preferredHeight: 25

                                        background: Rectangle {
                                            color: encoderController.encoderReverse ? Design.colButtonBackground : "white"
                                            border.width: 2
                                            border.color: Design.colFrameBorder
                                        }

                                        contentItem: Text {
                                            text: encoderController.encoderReverse ? qsTr("INVERSÉ") : qsTr("NORMAL")
                                            font.family: Fonts.robotoCondensed
                                            font.pointSize: 12
                                            font.bold: true
                                            color: encoderController.encoderReverse ? "white" : Design.colButtonDarkBackground
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        onClicked: encoderController.encoderReverse = !encoderController.encoderReverse
                                    }

                                    Rectangle {
                                        id: idCodeurValues
                                        Layout.preferredWidth: 212
                                        Layout.preferredHeight: 75

                                        color: "white"
                                        border.color: "black"
                                        border.width: 1

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 5
                                            spacing: 8

                                            Text {
                                                text: {
                                                    switch (currentVector.prodId) {
                                                    case 1:  return qsTr("UT POLE")
                                                    case 2:
                                                    case 6:  return qsTr("ALTISCAN")
                                                    default: return ""
                                                    }
                                                }
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                Layout.alignment: Qt.AlignHCenter
                                            }

                                            Text {
                                                text: {
                                                    switch (currentVector.prodId) {
                                                    case 1:  return qsTr("Diamètre roue : 56.80 mm")
                                                    case 2:
                                                    case 6:  return qsTr("Diamètre roue : 52.50 mm")
                                                    default: return ""
                                                    }
                                                }
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                Layout.alignment: Qt.AlignHCenter
                                            }

                                            Text {
                                                text: {
                                                    switch (currentVector.prodId) {
                                                    case 1:  return qsTr("Pas codeur : 8192")
                                                    case 2:
                                                    case 6:  return qsTr("Pas codeur : 1024")
                                                    default: return ""
                                                    }
                                                }
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                Layout.alignment: Qt.AlignHCenter
                                            }
                                        }

                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Rectangle {
                                    Layout.topMargin: 5
                                    Layout.bottomMargin: 5
                                    Layout.preferredWidth: 1
                                    Layout.fillHeight: true
                                    color : Design.colSeparator
                                }

                                Item { Layout.fillWidth: true }

                                TextMetrics {
                                    id: idCodeurColumn2Value
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 10
                                    font.bold: true
                                    text: "10000 mm"
                                }

                                ColumnLayout {
                                    id: idColumnRight2

                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    Text {
                                        text: qsTr("VALEUR RÉELLE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtCalibReelleMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationRealMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCalibReelleMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }


                                            onStepped: {
                                                if(encoderController.calibrationRealMm - 10 < 1){
                                                    encoderController.calibrationRealMm = 1
                                                }
                                                else
                                                    encoderController.calibrationRealMm = encoderController.calibrationRealMm - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCalibReelleMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationRealMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCalibReelleMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }


                                            onStepped: {
                                                if(encoderController.calibrationRealMm - 1 < 1){
                                                    encoderController.calibrationRealMm = 1
                                                }
                                                else
                                                    encoderController.calibrationRealMm = encoderController.calibrationRealMm - 1
                                            }
                                        }

                                        Item {
                                            id: idCalibReelleValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idCodeurColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idCalibReelleValue.editing
                                                text: encoderController.calibrationRealMm.toFixed(0) + " mm"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idCalibReelleValue.editing
                                                onDoubleClicked: {
                                                    idCalibReelleInput.text = encoderController.calibrationRealMm
                                                    idCalibReelleValue.editing = true
                                                    idCalibReelleInput.forceActiveFocus()
                                                    idCalibReelleInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idCalibReelleValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idCalibReelleInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idCalibReelleInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 5
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1; top: 10000 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        encoderController.calibrationRealMm = parseInt(text)
                                                        idCalibReelleValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idCalibReelleValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idCalibReelleValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCalibReellePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationRealMm < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCalibReellePlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(encoderController.calibrationRealMm + 1 > 10000){
                                                    encoderController.calibrationRealMm = 10000
                                                }
                                                else
                                                    encoderController.calibrationRealMm = encoderController.calibrationRealMm + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCalibReellePlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationRealMm < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCalibReellePlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(encoderController.calibrationRealMm + 10 > 10000){
                                                    encoderController.calibrationRealMm = 10000
                                                }
                                                else
                                                    encoderController.calibrationRealMm = encoderController.calibrationRealMm + 10
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("VALEUR PRÉVUE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtCodeurPrevueMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationEstimatedMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCodeurPrevueMinusMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("- -")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }


                                            onStepped: {
                                                if(encoderController.calibrationEstimatedMm - 10 < 1){
                                                    encoderController.calibrationEstimatedMm = 1
                                                }
                                                else
                                                    encoderController.calibrationEstimatedMm = encoderController.calibrationEstimatedMm - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCodeurPrevueMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationEstimatedMm > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCodeurPrevueMinus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("-")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(encoderController.calibrationEstimatedMm - 1 < 1){
                                                    encoderController.calibrationEstimatedMm = 1
                                                }
                                                else
                                                    encoderController.calibrationEstimatedMm = encoderController.calibrationEstimatedMm - 1
                                            }
                                        }


                                        Item {
                                            id: idCodeurPrevueValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idCodeurColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idCodeurPrevueValue.editing
                                                text: encoderController.calibrationEstimatedMm.toFixed(0) + " mm"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idCodeurPrevueValue.editing
                                                onDoubleClicked: {
                                                    idCodeurPrevueInput.text = encoderController.calibrationEstimatedMm
                                                    idCodeurPrevueValue.editing = true
                                                    idCodeurPrevueInput.forceActiveFocus()
                                                    idCodeurPrevueInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idCodeurPrevueValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idCodeurPrevueInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idCodeurPrevueInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 5
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1; top: 10000 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        encoderController.calibrationEstimatedMm = parseInt(text)
                                                        idCodeurPrevueValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idCodeurPrevueValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idCodeurPrevueValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCodeurPrevuePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationEstimatedMm < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCodeurPrevuePlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("+")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(encoderController.calibrationEstimatedMm + 1 > 10000){
                                                    encoderController.calibrationEstimatedMm = 10000
                                                }
                                                else
                                                    encoderController.calibrationEstimatedMm = encoderController.calibrationEstimatedMm + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtCodeurPrevuePlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: encoderController.calibrationEstimatedMm < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtCodeurPrevuePlusPlus.enabled ? 2 : 0.5
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("++")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onStepped: {
                                                if(encoderController.calibrationEstimatedMm + 10 > 10000){
                                                    encoderController.calibrationEstimatedMm = 10000
                                                }
                                                else
                                                    encoderController.calibrationEstimatedMm = encoderController.calibrationEstimatedMm + 10
                                            }
                                        }
                                    }

                                    Rectangle {
                                        Layout.preferredWidth: 360
                                        Layout.preferredHeight: 72
                                        color: "transparent"
                                        border.width: 2
                                        border.color: Design.colButtonDarkBackground

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            spacing: 8

                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                Layout.alignment: Qt.AlignVCenter
                                                Layout.leftMargin: 10
                                                spacing: 2

                                                Text {
                                                    text: qsTr("ÉTALONNER LE CODEUR")
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 13
                                                    font.bold: true
                                                    color: Design.colMainInk
                                                }

                                                Text {
                                                    text: qsTr("réf, ") + encoderController.calibrationRealMm.toFixed(0) + " mm"
                                                          + qsTr("  ·  lu ") + encoderController.calibrationEstimatedMm.toFixed(0) + " mm"
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 12
                                                    font.bold: true
                                                }
                                            }

                                            Button {
                                                id: idBtOkCodeur
                                                Layout.preferredWidth: 40
                                                Layout.preferredHeight: 40
                                                Layout.alignment: Qt.AlignVCenter

                                                background: Rectangle {
                                                    color: Design.colButtonDarkBackground
                                                    border.width: 2
                                                    border.color: Design.colFrameBorder
                                                }

                                                contentItem: Text {
                                                    text: qsTr("OK")
                                                    font.family: Fonts.robotoCondensed
                                                    font.pointSize: 10
                                                    font.bold: true
                                                    color: "white"
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                }
                                                onPressed: encoderController.calibrateNow(encoderController.calibrationRealMm,
                                                                                          encoderController.calibrationEstimatedMm)
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


            // ENREGISTRER / CHARGER - colonne verticale à gauche, toujours visible
            // Largeur fixe : fillWidth: false explicite, sinon le layout hérite de ses enfants
            ColumnLayout {
                Layout.fillWidth: false
                Layout.preferredWidth: 35
                Layout.fillHeight: true
                spacing: 5

                Button {
                    Layout.preferredWidth: 35
                    Layout.fillHeight: true

                    background: Rectangle {
                        color: Design.colOrange
                        border.width: 2
                        border.color: Design.colFrameBorder
                    }

                    // Texte tourné de -90° (lecture de bas en haut), comme le sélecteur
                    contentItem: Item {
                        Text {
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("ENREGISTRER")
                            font.family: Fonts.robotoCondensed
                            font.pointSize: 12
                            font.bold: true
                            color: Design.colButtonDarkBackground
                        }
                    }

                    onClicked: idSavePresetPopup.open()
                }

                Button {
                    Layout.preferredWidth: 35
                    Layout.fillHeight: true

                    background: Rectangle {
                        color: "white"
                        border.width: 2
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Item {
                        Text {
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("CHARGER")
                            font.family: Fonts.robotoCondensed
                            font.pointSize: 12
                            font.bold: true
                            color: Design.colButtonDarkBackground
                        }
                    }
                    onClicked: idLoadPresetPopup.open()
                }
            }
        }
    }
}
