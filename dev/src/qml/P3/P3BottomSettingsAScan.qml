import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    id: idRectBottomSettingsAScan

    // Shared state with P3SettingsBar / P3Top, set externally
    property var slCamSettings: null

    // Partie affichée : "capteur" (idLeftHalf) ou "portes" (idRightHalf)
    property string openingMode: "capteur"

    // Seuils porte 1 et 2 : [-100, 100] sans zero en mode RF (paramDWaveType = 3) sinon [1, 100]
    readonly property bool isRF: scanManager.SelectedFuncAScan.paramDWaveType === 3
    readonly property int gateHeightMin: isRF ? -100 : 1

    function clampGateHeight(value, direction) {
        var v = Math.max(gateHeightMin, Math.min(100, value));
        if( v === 0 ) v = direction < 0 ? -1 : 1;
        return v;
    }

    function stepGate0Height(delta) {
        var func = scanManager.SelectedFuncAScan;
        func.paramLGate0Height = clampGateHeight(func.paramLGate0Height + delta, delta);
    }

    function stepGate1Height(delta) {
        var func = scanManager.SelectedFuncAScan;
        func.paramLGate1Height = clampGateHeight(func.paramLGate1Height + delta, delta);
    }

    // En sortant du mode RF, les seuils porte 1 et 2 repartent à 1
    onIsRFChanged: {
        var func = scanManager.SelectedFuncAScan;
        if( func && !isRF ){
            func.paramLGate0Height = 1;
            func.paramLGate1Height = 1;
        }
    }

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
            if( name.length === 0 ) return;
            if( !confirmOverwrite && scanManager.ascanPresetExists(name) ) {
                confirmOverwrite = true;
                return;
            }
            if(scanManager.ascanSavePreset(name)){
                scanManager.ascanSaveUserParams();
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
                Layout.preferredHeight: 25
                Layout.fillWidth: true
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

    // CHARGER : liste des configurations sauvegardées
    Popup {
        id: idLoadPresetPopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 320
        height: 360
        modal: true
        focus: true
        padding: 0
        bottomPadding: 15

        property var presetNames: []

        // Liste relue à chaque ouverture : toujours à jour après un ENREGISTRER
        onOpened: presetNames = scanManager.ascanListPresets()

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

            ListView {
                id: idPresetList
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: 10
                Layout.leftMargin: 15
                Layout.rightMargin: 15
                clip: true
                spacing: 4
                model: idLoadPresetPopup.presetNames
                ScrollBar.vertical: ScrollBar { }

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
                        if( scanManager.ascanLoadPreset(modelData) ) idLoadPresetPopup.close();
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

    color: "#FFFFFF"
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

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8


            // Sélecteur vertical à droite : textes tournés de -90° (lecture de bas en haut)
            Rectangle {
                id: rectangleMenuAScaanOption
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
                        color: idRectBottomSettingsAScan.openingMode === "capteur" ? "#16253A" : "transparent"

                        Text {
                            font.family: "Roboto Condensed"
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("CAPTEUR")
                            font.pixelSize: 13
                            font.bold: idRectBottomSettingsAScan.openingMode === "capteur"
                            color: idRectBottomSettingsAScan.openingMode === "capteur" ? "#FFFFFF" : "#4B5563"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: idRectBottomSettingsAScan.openingMode = "capteur"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: idRectBottomSettingsAScan.openingMode === "portes" ? "#16253A" : "transparent"

                        Text {
                            font.family: "Roboto Condensed"
                            anchors.centerIn: parent
                            rotation: -90
                            text: qsTr("PORTES")
                            font.pixelSize: 13
                            font.bold: idRectBottomSettingsAScan.openingMode === "portes"
                            color: idRectBottomSettingsAScan.openingMode === "portes" ? "#FFFFFF" : "#4B5563"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: idRectBottomSettingsAScan.openingMode = "portes"
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

                    // PARAMÈTRES CAPTEUR - réglages A-scan (3 colonnes)
                    // Les deux parties sont exclusives : celle affichée occupe toute la largeur
                    Item {
                        id: idLeftHalf
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: idRectBottomSettingsAScan.openingMode === "capteur"
                        clip: true

                        // Largeur des boutons, réduite pour que les 3 colonnes tiennent dans la moitié

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 8

                            // COLONNES 1, 2 ET 3 - quatre espaces élastiques identiques :
                            // bord | colonne 1 | espace | colonne 2 | espace | colonne 3 | bord
                            RowLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignTop
                                spacing: 0

                                Item { Layout.fillWidth: true }

                                // COLONNE 1 - GAIN, CÉLÉRITÉ
                                ColumnLayout{
                                    id: idColumn1
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    TextMetrics {
                                        id: idTmColumn1Value
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        text: "230V"
                                    }

                                    // GAIN SETTINGS
                                    // COLUMN 1 - RAW 1
                                    Text {
                                        text: qsTr("GAIN")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtGainMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDGain > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtGainMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDGain - 10 < 0){
                                                    scanManager.SelectedFuncAScan.paramDGain = 0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtGainMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDGain > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtGainMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDGain - 1 < 0){
                                                    scanManager.SelectedFuncAScan.paramDGain = 0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain - 1
                                            }
                                        }

                                        Item {
                                            id: idGainValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn1Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idGainValue.editing
                                                text: scanManager.SelectedFuncAScan.paramDGain + " dB"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idGainValue.editing
                                                onDoubleClicked: {
                                                    idGainInput.text = scanManager.SelectedFuncAScan.paramDGain
                                                    idGainValue.editing = true
                                                    idGainInput.forceActiveFocus()
                                                    idGainInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idGainValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idGainInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idGainInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 2
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 0; top: 80 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramDGain = parseInt(text)
                                                        idGainValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idGainValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idGainValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtGainPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDGain < 80

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtGainPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDGain + 1 > 80){
                                                    scanManager.SelectedFuncAScan.paramDGain = 80
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtGainPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDGain < 80
                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtGainPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDGain + 10 > 80){
                                                    scanManager.SelectedFuncAScan.paramDGain = 80
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain + 10
                                            }
                                        }
                                    }


                                    // ORDRE DU FILTRE
                                    // COLUMN 3 - RAW 1
                                    Text {
                                        text: qsTr("VOLTAGE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtVoltageMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDVoltage > 10

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtVoltageMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDVoltage - 10 < 10){
                                                    scanManager.SelectedFuncAScan.paramDVoltage = 10
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDVoltage = scanManager.SelectedFuncAScan.paramDVoltage - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtVoltageMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDVoltage > 10

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtVoltageMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDVoltage - 1 < 10){
                                                    scanManager.SelectedFuncAScan.paramDVoltage = 10
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDVoltage = scanManager.SelectedFuncAScan.paramDVoltage - 1
                                            }
                                        }

                                        Item {
                                            id: idVoltageValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn1Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idVoltageValue.editing
                                                text: scanManager.SelectedFuncAScan.paramDVoltage + " V"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idVoltageValue.editing
                                                onDoubleClicked: {
                                                    idVoltageInput.text = scanManager.SelectedFuncAScan.paramDVoltage
                                                    idVoltageValue.editing = true
                                                    idVoltageInput.forceActiveFocus()
                                                    idVoltageInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idVoltageValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idVoltageInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idVoltageInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 3
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 10; top: 230 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramDVoltage = parseInt(text)
                                                        idVoltageValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idVoltageValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idVoltageValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtVoltagePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDVoltage < 230

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtVoltagePlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDVoltage + 1 > 230){
                                                    scanManager.SelectedFuncAScan.paramDVoltage = 230
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDVoltage = scanManager.SelectedFuncAScan.paramDVoltage + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtVoltagePlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDVoltage < 230

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtVoltagePlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDVoltage + 10 > 230){
                                                    scanManager.SelectedFuncAScan.paramDVoltage = 230
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDVoltage = scanManager.SelectedFuncAScan.paramDVoltage + 10
                                            }
                                        }
                                    }

                                    // ORDRE DU FILTRE
                                    // COLUMN 3 - RAW 1
                                    Text {
                                        text: qsTr("FRÉQ. ÉCHANTILLON")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        Button {
                                            id: idBtFrequence20Mhz
                                            Layout.preferredWidth: 49
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color: scanManager.SelectedFuncAScan.paramDSampleFreq === 2 ? "#16253A" : "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("20")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: scanManager.SelectedFuncAScan.paramDSampleFreq === 2 ? "white" : Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onClicked: {
                                                scanManager.SelectedFuncAScan.paramDSampleFreq = 2  // 20 MHz
                                            }
                                        }

                                        Button {
                                            id: idBtFrequence40Mhz
                                            Layout.preferredWidth: 49
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color:  scanManager.SelectedFuncAScan.paramDSampleFreq === 1 ? "#16253A" : "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("40")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: scanManager.SelectedFuncAScan.paramDSampleFreq === 1 ? "white" : Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onClicked: {
                                                scanManager.SelectedFuncAScan.paramDSampleFreq = 1  // 40 MHz
                                            }
                                        }

                                        Button {
                                            id: idBtFrequence80Mhz
                                            Layout.preferredWidth: 49
                                            Layout.preferredHeight: 25

                                            background: Rectangle {
                                                color:  scanManager.SelectedFuncAScan.paramDSampleFreq === 0 ? "#16253A" : "white"
                                                border.width: 2
                                                border.color: Design.colFrameBorder
                                            }

                                            contentItem: Text {
                                                text: qsTr("80")
                                                font.family: Fonts.robotoCondensed
                                                font.pointSize: 12
                                                font.bold: true
                                                color: scanManager.SelectedFuncAScan.paramDSampleFreq === 0 ? "white" : Design.colButtonDarkBackground
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            onClicked: {
                                                scanManager.SelectedFuncAScan.paramDSampleFreq = 0  // 80 MHz
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

                                // COLONNE 3 - ORDRE DU FILTRE, PROFONDEUR
                                ColumnLayout{
                                    id: idColumn2
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    // Largeur commune des valeurs (texte le plus long possible), pour que
                                    // les boutons soient à la même distance du centre sur chaque ligne
                                    TextMetrics {
                                        id: idTmColumn2Value
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        text: "10000 m/s"
                                    }

                                    // ORDRE DU FILTRE
                                    // COLUMN 3 - RAW 1
                                    Text {
                                        text: qsTr("LARGEUR IMPULSION")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtOrdreFiltreMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDPulseWidth > 18

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtOrdreFiltreMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDPulseWidth - 100 < 18){
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = 18
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = scanManager.SelectedFuncAScan.paramDPulseWidth - 100
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtOrdreFiltreMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDPulseWidth > 18

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtOrdreFiltreMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDPulseWidth - 1 < 18){
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = 18
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = scanManager.SelectedFuncAScan.paramDPulseWidth - 1
                                            }
                                        }

                                        Item {
                                            id: idOrdreFiltreValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idOrdreFiltreValue.editing
                                                text: scanManager.SelectedFuncAScan.paramDPulseWidth + " ns"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idOrdreFiltreValue.editing
                                                onDoubleClicked: {
                                                    idOrdreFiltreInput.text = scanManager.SelectedFuncAScan.paramDPulseWidth
                                                    idOrdreFiltreValue.editing = true
                                                    idOrdreFiltreInput.forceActiveFocus()
                                                    idOrdreFiltreInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idOrdreFiltreValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idOrdreFiltreInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idOrdreFiltreInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 4
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 18; top: 1600 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramDPulseWidth = parseInt(text)
                                                        idOrdreFiltreValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idOrdreFiltreValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idOrdreFiltreValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtOrdreFiltrePlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDPulseWidth < 1600

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtOrdreFiltrePlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDPulseWidth + 1 > 1600){
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = 1600
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = scanManager.SelectedFuncAScan.paramDPulseWidth + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtOrdreFiltrePlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramDPulseWidth < 1600

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtOrdreFiltrePlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDPulseWidth + 100 > 1600){
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = 1600
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramDPulseWidth = scanManager.SelectedFuncAScan.paramDPulseWidth + 100
                                            }
                                        }
                                    }


                                    // SPEED CONTROL
                                    // COLUMN 1 - RAW 4
                                    Text {
                                        text: qsTr("VITESSE US")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtSpeedMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLCelerity > 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtSpeedMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLCelerity - 100 < 1000){
                                                    scanManager.SelectedFuncAScan.paramLCelerity = 1000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLCelerity = scanManager.SelectedFuncAScan.paramLCelerity - 100
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtSpeedMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLCelerity > 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtSpeedMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLCelerity - 1 < 1000){
                                                    scanManager.SelectedFuncAScan.paramLCelerity = 1000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLCelerity = scanManager.SelectedFuncAScan.paramLCelerity - 1
                                            }
                                        }

                                        Item {
                                            id: idSpeedValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idSpeedValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLCelerity + " m/s"
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idSpeedValue.editing
                                                onDoubleClicked: {
                                                    idSpeedInput.text = scanManager.SelectedFuncAScan.paramLCelerity
                                                    idSpeedValue.editing = true
                                                    idSpeedInput.forceActiveFocus()
                                                    idSpeedInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idSpeedValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idSpeedInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idSpeedInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 5
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1000; top: 10000 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramLCelerity = parseInt(text)
                                                        idSpeedValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idSpeedValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idSpeedValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtSpeedPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLCelerity < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtSpeedPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLCelerity + 1 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLCelerity = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLCelerity = scanManager.SelectedFuncAScan.paramLCelerity + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtSpeedPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLCelerity < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtSpeedPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLCelerity + 100 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLCelerity = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLCelerity = scanManager.SelectedFuncAScan.paramLCelerity + 100
                                            }
                                        }
                                    }

                                    // PROFONDEUR
                                    // COLUMN 3 - RAW 4
                                    Text {
                                        text: qsTr("LISSAGE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtProfondeurMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLLowPassFilter > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtProfondeurMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLLowPassFilter - 10 < 0){
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = 0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = scanManager.SelectedFuncAScan.paramLLowPassFilter - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtProfondeurMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLLowPassFilter > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtProfondeurMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLLowPassFilter - 1 < 0){
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = 0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = scanManager.SelectedFuncAScan.paramLLowPassFilter - 1
                                            }
                                        }

                                        Item {
                                            id: idProfondeurValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idProfondeurValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLLowPassFilter
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idProfondeurValue.editing
                                                onDoubleClicked: {
                                                    idProfondeurInput.text = scanManager.SelectedFuncAScan.paramLLowPassFilter
                                                    idProfondeurValue.editing = true
                                                    idProfondeurInput.forceActiveFocus()
                                                    idProfondeurInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idProfondeurValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idProfondeurInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idProfondeurInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 2
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 0; top: 50 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramLLowPassFilter = parseInt(text)
                                                        idProfondeurValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idProfondeurValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idProfondeurValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtProfondeurPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLLowPassFilter < 50

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtProfondeurPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLLowPassFilter + 1 > 50){
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = 50
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = scanManager.SelectedFuncAScan.paramLLowPassFilter + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtProfondeurPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLLowPassFilter < 50

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtProfondeurPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLLowPassFilter + 10 > 50){
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = 50
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLLowPassFilter = scanManager.SelectedFuncAScan.paramLLowPassFilter + 10
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("RETARD PALPEUR")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        id: rowRetardPalpeur
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtRetardMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLProbeDelayMm > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtRetardMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLProbeDelayMm - 10 < 0){
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = 0.0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = Math.round((scanManager.SelectedFuncAScan.paramLProbeDelayMm - 10) * 10) / 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtRetardMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLProbeDelayMm > 0

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtRetardMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLProbeDelayMm - 0.1 < 0){
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = 0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = Math.round((scanManager.SelectedFuncAScan.paramLProbeDelayMm - 0.1) * 10) / 10
                                            }
                                        }

                                        Item {
                                            id: idRetardValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmColumn2Value.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idRetardValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLProbeDelayMm.toFixed(1)
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idRetardValue.editing
                                                onDoubleClicked: {
                                                    idRetardInput.text = scanManager.SelectedFuncAScan.paramLProbeDelayMm.toFixed(1)
                                                    idRetardValue.editing = true
                                                    idRetardInput.forceActiveFocus()
                                                    idRetardInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idRetardValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idRetardInput.valid ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idRetardInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 6
                                                    selectByMouse: true
                                                    inputMethodHints: Qt.ImhFormattedNumbersOnly

                                                    // Validated by hand (no validator): one decimal, comma or dot
                                                    readonly property real value: parseFloat(text.replace(",", "."))
                                                    readonly property bool valid: /^\d{1,4}([.,]\d)?$/.test(text)
                                                                                  && value >= 0 && value <= 1000

                                                    // Keep only digits and a decimal separator
                                                    onTextChanged: {
                                                        var filtered = text.replace(/[^0-9.,]/g, "")
                                                        if(filtered !== text)
                                                            text = filtered
                                                    }

                                                    onAccepted: {
                                                        if(!valid)
                                                            return
                                                        scanManager.SelectedFuncAScan.paramLProbeDelayMm = Math.round(value * 10) / 10
                                                        idRetardValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idRetardValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idRetardValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtRetardPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLProbeDelayMm < 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtRetardPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLProbeDelayMm + 0.1 > 1000){
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = 1000.0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = Math.round((scanManager.SelectedFuncAScan.paramLProbeDelayMm + 0.1) * 10) / 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtRetardPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLProbeDelayMm < 1000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtRetardPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLProbeDelayMm + 10 > 1000){
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = 1000.0
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLProbeDelayMm = Math.round((scanManager.SelectedFuncAScan.paramLProbeDelayMm + 10) * 10) / 10
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

                                ColumnLayout {
                                    id: idLeftColumn3
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    Text {
                                        text: qsTr("ÉLÉMENT")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    ComboBox{
                                        id: elementSelectedCell
                                        Layout.preferredWidth: rowRetardPalpeur.width
                                        Layout.preferredHeight: 25
                                        font.pixelSize: 12
                                        font.bold: true
                                        font.family: Fonts.robotoCondensed
                                        model: ["MONO-ÉLÉMENT", "BI-ÉLÉMENT"]
                                        // paramDUtElement : 1 = mono, 2 = bi (0 = non défini, traité comme mono)
                                        currentIndex: scanManager.SelectedFuncAScan.paramDUtElement === 2 ? 1 : 0
                                        onActivated: {
                                            scanManager.SelectedFuncAScan.paramDUtElement = index + 1
                                        }
                                        // Texte de la cellule : toujours noir, lisible
                                        contentItem:         Text {
                                            font.family: "Roboto Condensed"
                                            text: elementSelectedCell.displayText
                                            font.pixelSize: 12
                                            font.bold: true
                                            leftPadding: 6
                                            color: "#0D2B38"
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: elementSelectedCell.activeFocus ? "#E6EFF3" : "#FFFFFF"
                                            border.width: 1
                                            border.color: elementSelectedCell.activeFocus ? "#0D2B38" : "#C7CBD1"
                                        }

                                        // Menu déroulant : fond blanc
                                        popup: Popup {
                                            y: elementSelectedCell.height
                                            width: elementSelectedCell.width
                                            implicitHeight: contentItem.implicitHeight
                                            padding: 1

                                            contentItem: ListView {
                                                clip: true
                                                implicitHeight: contentHeight
                                                model: elementSelectedCell.popup.visible ? elementSelectedCell.delegateModel : null
                                                currentIndex: elementSelectedCell.highlightedIndex
                                                ScrollIndicator.vertical: ScrollIndicator {}
                                            }

                                            background: Rectangle {
                                                color: "#FFFFFF"
                                                border.color: "#C7CBD1"
                                                border.width: 1
                                            }
                                        }

                                        // Chaque ligne du menu : fond blanc, texte noir
                                        delegate: ItemDelegate {
                                            id: interReferentielDelegate
                                            width: elementSelectedCell.width
                                            highlighted: elementSelectedCell.highlightedIndex === index
                                            contentItem:         Text {
                                                font.family: "Roboto Condensed"
                                                text: modelData
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#0D2B38"
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }
                                            background: Rectangle {
                                                color: interReferentielDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("ALTERNANCE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }


                                    ComboBox{
                                        id: alternanceSelectedCell
                                        Layout.preferredWidth: rowRetardPalpeur.width
                                        Layout.preferredHeight: 25
                                        font.pixelSize: 12
                                        font.bold: true
                                        font.family: Fonts.robotoCondensed
                                        model: ["REDRESSÉE", "NEGATIVE", "POSITIVE", "RF"]
                                        currentIndex: scanManager.SelectedFuncAScan.paramDWaveType
                                        onActivated: {
                                            scanManager.SelectedFuncAScan.paramDWaveType = index
                                        }
                                        // Texte de la cellule : toujours noir, lisible
                                        contentItem:         Text {
                                            font.family: "Roboto Condensed"
                                            text: alternanceSelectedCell.displayText
                                            font.pixelSize: 12
                                            font.bold: true
                                            leftPadding: 6
                                            color: "#0D2B38"
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: alternanceSelectedCell.activeFocus ? "#E6EFF3" : "#FFFFFF"
                                            border.width: 1
                                            border.color: alternanceSelectedCell.activeFocus ? "#0D2B38" : "#C7CBD1"
                                        }

                                        // Menu déroulant : fond blanc
                                        popup: Popup {
                                            y: alternanceSelectedCell.height
                                            width: alternanceSelectedCell.width
                                            implicitHeight: contentItem.implicitHeight
                                            padding: 1

                                            contentItem: ListView {
                                                clip: true
                                                implicitHeight: contentHeight
                                                model: alternanceSelectedCell.popup.visible ? alternanceSelectedCell.delegateModel : null
                                                currentIndex: alternanceSelectedCell.highlightedIndex
                                                ScrollIndicator.vertical: ScrollIndicator {}
                                            }

                                            background: Rectangle {
                                                color: "#FFFFFF"
                                                border.color: "#C7CBD1"
                                                border.width: 1
                                            }
                                        }

                                        // Chaque ligne du menu : fond blanc, texte noir
                                        delegate: ItemDelegate {
                                            id: interAlternanceDelegate
                                            width: alternanceSelectedCell.width
                                            highlighted: alternanceSelectedCell.highlightedIndex === index
                                            contentItem:         Text {
                                                font.family: "Roboto Condensed"
                                                text: modelData
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#0D2B38"
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }
                                            background: Rectangle {
                                                color: interAlternanceDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("FILTRE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    ComboBox{
                                        id: filtreSelectedCell
                                        Layout.preferredWidth: rowRetardPalpeur.width
                                        Layout.preferredHeight: 25
                                        font.pixelSize: 12
                                        font.bold: true
                                        font.family: Fonts.robotoCondensed
                                        model: ["1.25MHz", "2.5MHz", "5MHz", "10MHz", "BROAD BAND"]
                                        currentIndex: scanManager.SelectedFuncAScan.paramDFilterMode
                                        onActivated: {
                                            scanManager.SelectedFuncAScan.paramDFilterMode = index
                                        }
                                        // Texte de la cellule : toujours noir, lisible
                                        contentItem:         Text {
                                            font.family: "Roboto Condensed"
                                            text: filtreSelectedCell.displayText
                                            font.pixelSize: 12
                                            font.bold: true
                                            leftPadding: 6
                                            color: "#0D2B38"
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: filtreSelectedCell.activeFocus ? "#E6EFF3" : "#FFFFFF"
                                            border.width: 1
                                            border.color: filtreSelectedCell.activeFocus ? "#0D2B38" : "#C7CBD1"
                                        }

                                        // Menu déroulant : fond blanc
                                        popup: Popup {
                                            y: filtreSelectedCell.height
                                            width: filtreSelectedCell.width
                                            implicitHeight: contentItem.implicitHeight
                                            padding: 1

                                            contentItem: ListView {
                                                clip: true
                                                implicitHeight: contentHeight
                                                model: filtreSelectedCell.popup.visible ? filtreSelectedCell.delegateModel : null
                                                currentIndex: filtreSelectedCell.highlightedIndex
                                                ScrollIndicator.vertical: ScrollIndicator {}
                                            }

                                            background: Rectangle {
                                                color: "#FFFFFF"
                                                border.color: "#C7CBD1"
                                                border.width: 1
                                            }
                                        }

                                        // Chaque ligne du menu : fond blanc, texte noir
                                        delegate: ItemDelegate {
                                            id: interFiltreDelegate
                                            width: filtreSelectedCell.width
                                            highlighted: filtreSelectedCell.highlightedIndex === index
                                            contentItem:         Text {
                                                font.family: "Roboto Condensed"
                                                text: modelData
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#0D2B38"
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }
                                            background: Rectangle {
                                                color: interFiltreDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                            }
                                        }
                                    }
                                    Item { Layout.fillHeight: true }
                                }
                                Item { Layout.fillWidth: true }
                            }
                            Item { Layout.fillHeight: true}
                        }
                    }

                    // PARAMÈTRES PORTES - portes 1 et 2, porte de bruit
                    Item {
                        id: idRightHalf
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: idRectBottomSettingsAScan.openingMode === "portes"
                        clip: true

                        TextMetrics {
                            id: idTmRightValue
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 10
                            font.bold: true
                            text: "10000"
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 8

                            // PORTES 1 ET 2 - trois espaces élastiques identiques :
                            // bord | porte 1 | espace | porte 2 | bord
                            RowLayout {
                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignTop
                                spacing: 0

                                Item { Layout.fillWidth: true }

                                // COLONNE PORTE 1
                                ColumnLayout{
                                    id: idRightColumn1
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8


                                    // Debut POrte 1 Setting
                                    // COLUMN 1 - RAW 2
                                    Text {
                                        text: qsTr("DÉBUT PORTE 1")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        id: rowDebutNoise0
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtStartGreenMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Start > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartGreenMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Start - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = scanManager.SelectedFuncAScan.paramLGate0Start - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartGreenMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Start > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartGreenMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Start - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = scanManager.SelectedFuncAScan.paramLGate0Start - 1
                                            }
                                        }

                                        Item {
                                            id: idStartGreenValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idStartGreenValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate0Start
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idStartGreenValue.editing
                                                onDoubleClicked: {
                                                    idStartGreenInput.text = scanManager.SelectedFuncAScan.paramLGate0Start
                                                    idStartGreenValue.editing = true
                                                    idStartGreenInput.forceActiveFocus()
                                                    idStartGreenInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idStartGreenValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idStartGreenInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idStartGreenInput
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
                                                        scanManager.SelectedFuncAScan.paramLGate0Start = parseInt(text)
                                                        idStartGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idStartGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idStartGreenValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartGreenPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Start < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartGreenPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Start + 1 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = scanManager.SelectedFuncAScan.paramLGate0Start + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartGreenPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Start < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartGreenPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Start + 10 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Start = scanManager.SelectedFuncAScan.paramLGate0Start + 10
                                            }
                                        }
                                    }
                                    // LARGEUR PORTE 1
                                    // COLUMN 2 - RAW 2
                                    Text {
                                        text: qsTr("LARGEUR PORTE 1")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtWidthGreenMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Width > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthGreenMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Width - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = scanManager.SelectedFuncAScan.paramLGate0Width - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthGreenMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Width > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthGreenMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Width - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = 1
                                                }
                                                else{
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = scanManager.SelectedFuncAScan.paramLGate0Width - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idWidthGreenValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idWidthGreenValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate0Width
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idWidthGreenValue.editing
                                                onDoubleClicked: {
                                                    idWidthGreenInput.text = scanManager.SelectedFuncAScan.paramLGate0Width
                                                    idWidthGreenValue.editing = true
                                                    idWidthGreenInput.forceActiveFocus()
                                                    idWidthGreenInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idWidthGreenValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idWidthGreenInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idWidthGreenInput
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
                                                        scanManager.SelectedFuncAScan.paramLGate0Width = parseInt(text)
                                                        idWidthGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idWidthGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idWidthGreenValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthGreenPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Width < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthGreenPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Width + 1 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = scanManager.SelectedFuncAScan.paramLGate0Width + 1
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthGreenPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Width < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthGreenPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate0Width + 10 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = 10000
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate0Width = scanManager.SelectedFuncAScan.paramLGate0Width + 10
                                            }
                                        }
                                    }

                                    // SEUIL PORTE 1
                                    // COLUMN 3 - RAW 2
                                    Text {
                                        text: qsTr("SEUIL PORTE 1")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtHeightGreenMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Height > idRectBottomSettingsAScan.gateHeightMin

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightGreenMinusMinus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate0Height(-10)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightGreenMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Height > idRectBottomSettingsAScan.gateHeightMin

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightGreenMinus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate0Height(-1)
                                            }
                                        }

                                        Item {
                                            id: idHeightGreenValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idHeightGreenValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate0Height
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idHeightGreenValue.editing
                                                onDoubleClicked: {
                                                    idHeightGreenInput.text = scanManager.SelectedFuncAScan.paramLGate0Height
                                                    idHeightGreenValue.editing = true
                                                    idHeightGreenInput.forceActiveFocus()
                                                    idHeightGreenInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idHeightGreenValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idHeightGreenInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idHeightGreenInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 4
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: idRectBottomSettingsAScan.gateHeightMin; top: 100 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        var value = parseInt(text)
                                                        if(value === 0)
                                                            return
                                                        scanManager.SelectedFuncAScan.paramLGate0Height = value
                                                        idHeightGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idHeightGreenValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idHeightGreenValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightGreenPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Height < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightGreenPlus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate0Height(1)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightGreenPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate0Height < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightGreenPlusPlus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate0Height(10)
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("OPTION PORTE 1")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    ComboBox{
                                        id: porte1OptionCell
                                        Layout.preferredWidth: rowDebutNoise0.width
                                        Layout.preferredHeight: 25
                                        font.pixelSize: 12
                                        font.bold: true
                                        font.family: Fonts.robotoCondensed
                                        model: ["PREMIERE MAXIMUM", "MAXIMUM ABSOLU", "FLANC MONTANT"]
                                        currentIndex: scanManager.SelectedFuncAScan.paramDGate0Option
                                        onActivated: {
                                            scanManager.SelectedFuncAScan.paramDGate0Option = index
                                        }
                                        // Texte de la cellule : toujours noir, lisible
                                        contentItem:         Text {
                                            font.family: "Roboto Condensed"
                                            text: porte1OptionCell.displayText
                                            font.pixelSize: 12
                                            font.bold: true
                                            leftPadding: 6
                                            color: "#0D2B38"
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: porte1OptionCell.activeFocus ? "#E6EFF3" : "#FFFFFF"
                                            border.width: 1
                                            border.color: porte1OptionCell.activeFocus ? "#0D2B38" : "#C7CBD1"
                                        }

                                        // Menu déroulant : fond blanc
                                        popup: Popup {
                                            y: porte1OptionCell.height
                                            width: porte1OptionCell.width
                                            implicitHeight: contentItem.implicitHeight
                                            padding: 1

                                            contentItem: ListView {
                                                clip: true
                                                implicitHeight: contentHeight
                                                model: porte1OptionCell.popup.visible ? porte1OptionCell.delegateModel : null
                                                currentIndex: porte1OptionCell.highlightedIndex
                                                ScrollIndicator.vertical: ScrollIndicator {}
                                            }

                                            background: Rectangle {
                                                color: "#FFFFFF"
                                                border.color: "#C7CBD1"
                                                border.width: 1
                                            }
                                        }

                                        // Chaque ligne du menu : fond blanc, texte noir
                                        delegate: ItemDelegate {
                                            id: interPorte1OptionDelegate
                                            width: porte1OptionCell.width
                                            highlighted: porte1OptionCell.highlightedIndex === index
                                            contentItem:         Text {
                                                font.family: "Roboto Condensed"
                                                text: modelData
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#0D2B38"
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }
                                            background: Rectangle {
                                                color: interPorte1OptionDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
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

                                // COLONNE PORTE 2
                                ColumnLayout{
                                    id: idRightColumn2
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    // DEBUT PORTE ROUGE 2
                                    // COLUMN 1 - RAW 3
                                    Text {
                                        text: qsTr("DÉBUT PORTE 2")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtStartRedMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Start > 1 && !scanManager.SelectedFuncAScan.paramDAutoSyncGates

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartRedMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate1Start - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate1Start = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate1Start = scanManager.SelectedFuncAScan.paramLGate1Start - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartRedMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Start > 1 && !scanManager.SelectedFuncAScan.paramDAutoSyncGates

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartRedMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate1Start - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate1Start = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate1Start = scanManager.SelectedFuncAScan.paramLGate1Start - 1
                                            }
                                        }

                                        Item {
                                            id: idStartRedValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idStartRedValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate1Start
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idStartRedValue.editing && !scanManager.SelectedFuncAScan.paramDAutoSyncGates
                                                onDoubleClicked: {
                                                    idStartRedInput.text = scanManager.SelectedFuncAScan.paramLGate1Start
                                                    idStartRedValue.editing = true
                                                    idStartRedInput.forceActiveFocus()
                                                    idStartRedInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idStartRedValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idStartRedInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idStartRedInput
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
                                                        scanManager.SelectedFuncAScan.paramLGate1Start = parseInt(text)
                                                        idStartRedValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idStartRedValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idStartRedValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartRedPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Start < 10000 && !scanManager.SelectedFuncAScan.paramDAutoSyncGates


                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartRedPlus.enabled ? 2 : 0.5
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
                                                scanManager.SelectedFuncAScan.paramLGate1Start = Math.min(scanManager.SelectedFuncAScan.paramLGate1Start + 1, 10000)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtStartRedPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Start < 10000 && !scanManager.SelectedFuncAScan.paramDAutoSyncGates


                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtStartRedPlusPlus.enabled ? 2 : 0.5
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
                                                scanManager.SelectedFuncAScan.paramLGate1Start = Math.min(scanManager.SelectedFuncAScan.paramLGate1Start + 10, 10000)
                                            }
                                        }
                                    }

                                    // LARGEUR PORTE ROUGE 2
                                    // COLUMN 2 - RAW 3
                                    Text {
                                        text: qsTr("LARGEUR PORTE 2")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtWidthRedMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Width > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthRedMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate1Width - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate1Width = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate1Width = scanManager.SelectedFuncAScan.paramLGate1Width - 10
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthRedMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Width > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthRedMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGate1Width - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGate1Width = 1
                                                }
                                                else
                                                    scanManager.SelectedFuncAScan.paramLGate1Width = scanManager.SelectedFuncAScan.paramLGate1Width - 1
                                            }
                                        }

                                        Item {
                                            id: idWidthRedValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idWidthRedValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate1Width
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idWidthRedValue.editing
                                                onDoubleClicked: {
                                                    idWidthRedInput.text = scanManager.SelectedFuncAScan.paramLGate1Width
                                                    idWidthRedValue.editing = true
                                                    idWidthRedInput.forceActiveFocus()
                                                    idWidthRedInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idWidthRedValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idWidthRedInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idWidthRedInput
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
                                                        scanManager.SelectedFuncAScan.paramLGate1Width = parseInt(text)
                                                        idWidthRedValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idWidthRedValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idWidthRedValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthRedPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Width < 10000


                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthRedPlus.enabled ? 2 : 0.5
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
                                                scanManager.SelectedFuncAScan.paramLGate1Width = Math.min(scanManager.SelectedFuncAScan.paramLGate1Width + 1, 10000)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtWidthRedPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25
                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Width < 10000


                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtWidthRedPlusPlus.enabled ? 2 : 0.5
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
                                                scanManager.SelectedFuncAScan.paramLGate1Width = Math.min(scanManager.SelectedFuncAScan.paramLGate1Width + 10, 10000)
                                            }
                                        }
                                    }

                                    // SEUIL PORTE ROUGE 2
                                    // COLUMN 3 - RAW 3
                                    Text {
                                        text: qsTr("SEUIL PORTE 2")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout{
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtHeightRedMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Height > idRectBottomSettingsAScan.gateHeightMin

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightRedMinusMinus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate1Height(-10)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightRedMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Height > idRectBottomSettingsAScan.gateHeightMin

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightRedMinus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate1Height(-1)
                                            }
                                        }

                                        Item {
                                            id: idHeightRedValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idHeightRedValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGate1Height
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idHeightRedValue.editing
                                                onDoubleClicked: {
                                                    idHeightRedInput.text = scanManager.SelectedFuncAScan.paramLGate1Height
                                                    idHeightRedValue.editing = true
                                                    idHeightRedInput.forceActiveFocus()
                                                    idHeightRedInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idHeightRedValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idHeightRedInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idHeightRedInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 4
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: idRectBottomSettingsAScan.gateHeightMin; top: 100 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        var value = parseInt(text)
                                                        if(value === 0)
                                                            return
                                                        scanManager.SelectedFuncAScan.paramLGate1Height = value
                                                        idHeightRedValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idHeightRedValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idHeightRedValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightRedPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Height < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightRedPlus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate1Height(1)
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtHeightRedPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGate1Height < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtHeightRedPlusPlus.enabled ? 2 : 0.5
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
                                                idRectBottomSettingsAScan.stepGate1Height(10)
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("OPTION PORTE 2")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    ComboBox{
                                        id: porte2OptionCell
                                        Layout.preferredWidth: rowDebutNoise0.width
                                        Layout.preferredHeight: 25
                                        font.pixelSize: 12
                                        font.bold: true
                                        font.family: Fonts.robotoCondensed
                                        model: ["PREMIERE MAXIMUM", "MAXIMUM ABSOLU", "FLANC MONTANT"]
                                        currentIndex: scanManager.SelectedFuncAScan.paramDGate1Option
                                        onActivated: {
                                            scanManager.SelectedFuncAScan.paramDGate1Option = index
                                        }
                                        // Texte de la cellule : toujours noir, lisible
                                        contentItem:         Text {
                                            font.family: "Roboto Condensed"
                                            text: porte2OptionCell.displayText
                                            font.pixelSize: 12
                                            font.bold: true
                                            leftPadding: 6
                                            color: "#0D2B38"
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        background: Rectangle {
                                            color: porte2OptionCell.activeFocus ? "#E6EFF3" : "#FFFFFF"
                                            border.width: 1
                                            border.color: porte2OptionCell.activeFocus ? "#0D2B38" : "#C7CBD1"
                                        }

                                        // Menu déroulant : fond blanc
                                        popup: Popup {
                                            y: porte2OptionCell.height
                                            width: porte2OptionCell.width
                                            implicitHeight: contentItem.implicitHeight
                                            padding: 1

                                            contentItem: ListView {
                                                clip: true
                                                implicitHeight: contentHeight
                                                model: porte2OptionCell.popup.visible ? porte2OptionCell.delegateModel : null
                                                currentIndex: porte2OptionCell.highlightedIndex
                                                ScrollIndicator.vertical: ScrollIndicator {}
                                            }

                                            background: Rectangle {
                                                color: "#FFFFFF"
                                                border.color: "#C7CBD1"
                                                border.width: 1
                                            }
                                        }

                                        // Chaque ligne du menu : fond blanc, texte noir
                                        delegate: ItemDelegate {
                                            id: interPorte2OptionDelegate
                                            width: porte2OptionCell.width
                                            highlighted: porte2OptionCell.highlightedIndex === index
                                            contentItem:         Text {
                                                font.family: "Roboto Condensed"
                                                text: modelData
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#0D2B38"
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }
                                            background: Rectangle {
                                                color: interPorte2OptionDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("AUTO SYNC PORTE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Button {
                                        id: idBtActiveAutoSync
                                        Layout.preferredWidth: rowDebutNoise0.width
                                        Layout.preferredHeight: 25

                                        background: Rectangle {
                                            color: scanManager.SelectedFuncAScan.paramDAutoSyncGates ? "#16253A" : "white"
                                            border.width: 2
                                            border.color: Design.colFrameBorder
                                        }

                                        contentItem: Text {
                                            text: if(scanManager.SelectedFuncAScan.paramDAutoSyncGates){
                                                      return qsTr("ACTIF")
                                                  }else{
                                                      return qsTr("INACTIF")
                                                  }
                                            font.family: Fonts.robotoCondensed
                                            font.pointSize: 12
                                            font.bold: true
                                            color: scanManager.SelectedFuncAScan.paramDAutoSyncGates ? "white" : Design.colButtonDarkBackground
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        onClicked: {
                                            scanManager.SelectedFuncAScan.paramDAutoSyncGates = !scanManager.SelectedFuncAScan.paramDAutoSyncGates
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

                                ColumnLayout {
                                    id: idRightColumn3
                                    Layout.alignment: Qt.AlignTop
                                    spacing: 8

                                    Text {
                                        text: qsTr("DÉBUT NOISE GATE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtNoiseDebutMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseStart > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseDebutMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseStart - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = 1
                                                }else{
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = scanManager.SelectedFuncAScan.paramLGateNoiseStart - 10
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseDebutMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseStart > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseDebutMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseStart - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = 1
                                                }else{
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = scanManager.SelectedFuncAScan.paramLGateNoiseStart - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idNoiseStartValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idNoiseStartValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGateNoiseStart
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idNoiseStartValue.editing
                                                onDoubleClicked: {
                                                    idNoiseStartInput.text = scanManager.SelectedFuncAScan.paramLGateNoiseStart
                                                    idNoiseStartValue.editing = true
                                                    idNoiseStartInput.forceActiveFocus()
                                                    idNoiseStartInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idNoiseStartValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idNoiseStartInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idNoiseStartInput
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
                                                        scanManager.SelectedFuncAScan.paramLGateNoiseStart = parseInt(text)
                                                        idNoiseStartValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idNoiseStartValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idNoiseStartValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseDebutPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseStart < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseDebutPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseStart + 1 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = 10000
                                                }else{
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = scanManager.SelectedFuncAScan.paramLGateNoiseStart + 1
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseDebutPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseStart < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseDebutPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseStart + 10 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = 10000
                                                }else{
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseStart = scanManager.SelectedFuncAScan.paramLGateNoiseStart + 10
                                                }
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("LARGEUR NOISE GATE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtNoiseLargeurMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseWidth > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseLargeurMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseWidth - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = 1
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = scanManager.SelectedFuncAScan.paramLGateNoiseWidth - 10
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseLargeurMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseWidth > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseLargeurMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseWidth - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = 1
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = scanManager.SelectedFuncAScan.paramLGateNoiseWidth - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idNoiseWidthValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idNoiseWidthValue.editing
                                                text: scanManager.SelectedFuncAScan.paramLGateNoiseWidth
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idNoiseWidthValue.editing
                                                onDoubleClicked: {
                                                    idNoiseWidthInput.text = scanManager.SelectedFuncAScan.paramLGateNoiseWidth
                                                    idNoiseWidthValue.editing = true
                                                    idNoiseWidthInput.forceActiveFocus()
                                                    idNoiseWidthInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idNoiseWidthValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idNoiseWidthInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idNoiseWidthInput
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
                                                        scanManager.SelectedFuncAScan.paramLGateNoiseWidth = parseInt(text)
                                                        idNoiseWidthValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idNoiseWidthValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idNoiseWidthValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseLargeurPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseWidth < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseLargeurPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseWidth + 1 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = 10000
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = scanManager.SelectedFuncAScan.paramLGateNoiseWidth + 1
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseLargeurPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramLGateNoiseWidth < 10000

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseLargeurPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramLGateNoiseWidth + 10 > 10000){
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = 10000
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramLGateNoiseWidth = scanManager.SelectedFuncAScan.paramLGateNoiseWidth + 10
                                                }
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("SEUIL NOISE GATE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        Layout.alignment: Qt.AlignHCenter
                                        spacing: 8

                                        P3RepeatButton {
                                            id: idBtNoiseSeuilMinusMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramDAutoGainThreshold > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseSeuilMinusMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDAutoGainThreshold - 10 < 1){
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = 1
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = scanManager.SelectedFuncAScan.paramDAutoGainThreshold - 10
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseSeuilMinus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramDAutoGainThreshold > 1

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseSeuilMinus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDAutoGainThreshold - 1 < 1){
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = 1
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = scanManager.SelectedFuncAScan.paramDAutoGainThreshold - 1
                                                }
                                            }
                                        }

                                        Item {
                                            id: idNoiseThresholdValue
                                            property bool editing: false

                                            Layout.alignment: Qt.AlignHCenter
                                            Layout.preferredWidth: idTmRightValue.width
                                            Layout.preferredHeight: 25

                                            Text {
                                                anchors.fill: parent
                                                visible: !idNoiseThresholdValue.editing
                                                text: scanManager.SelectedFuncAScan.paramDAutoGainThreshold
                                                font.family: Fonts.robotoCondensed
                                                font.pixelSize: 10
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                enabled: !idNoiseThresholdValue.editing
                                                onDoubleClicked: {
                                                    idNoiseThresholdInput.text = scanManager.SelectedFuncAScan.paramDAutoGainThreshold
                                                    idNoiseThresholdValue.editing = true
                                                    idNoiseThresholdInput.forceActiveFocus()
                                                    idNoiseThresholdInput.selectAll()
                                                }
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                visible: idNoiseThresholdValue.editing
                                                color: "white"
                                                border.width: 2
                                                border.color: idNoiseThresholdInput.acceptableInput ? Design.colFrameBorder : "#C62828"

                                                TextInput {
                                                    id: idNoiseThresholdInput
                                                    anchors.fill: parent
                                                    anchors.margins: 2
                                                    font.family: Fonts.robotoCondensed
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                    maximumLength: 3
                                                    selectByMouse: true
                                                    validator: IntValidator { bottom: 1; top: 100 }

                                                    // Only emitted when the text satisfies the validator
                                                    onAccepted: {
                                                        scanManager.SelectedFuncAScan.paramDAutoGainThreshold = parseInt(text)
                                                        idNoiseThresholdValue.editing = false
                                                        focus = false
                                                    }

                                                    Keys.onEscapePressed: {
                                                        idNoiseThresholdValue.editing = false
                                                        focus = false
                                                    }

                                                    onActiveFocusChanged: {
                                                        if(!activeFocus)
                                                            idNoiseThresholdValue.editing = false
                                                    }
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseSeuilPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramDAutoGainThreshold < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseSeuilPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDAutoGainThreshold + 1 > 100){
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = 100
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = scanManager.SelectedFuncAScan.paramDAutoGainThreshold + 1
                                                }
                                            }
                                        }

                                        P3RepeatButton {
                                            id: idBtNoiseSeuilPlusPlus
                                            Layout.preferredWidth: 25
                                            Layout.preferredHeight: 25

                                            enabled: scanManager.SelectedFuncAScan.paramDAutoGainThreshold < 100

                                            background: Rectangle {
                                                color: "white"
                                                border.width: idBtNoiseSeuilPlusPlus.enabled ? 2 : 0.5
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
                                                if(scanManager.SelectedFuncAScan.paramDAutoGainThreshold + 10 > 100){
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = 100
                                                }else {
                                                    scanManager.SelectedFuncAScan.paramDAutoGainThreshold = scanManager.SelectedFuncAScan.paramDAutoGainThreshold + 10
                                                }
                                            }
                                        }
                                    }

                                    Text {
                                        text: qsTr("NOISE GATE")
                                        font.family: Fonts.robotoCondensed
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Button {
                                        id: idBtActiveNoiseGate
                                        Layout.preferredWidth: rowDebutNoise0.width
                                        Layout.preferredHeight: 25

                                        background: Rectangle {
                                            color: scanManager.SelectedFuncAScan.paramDAutoGain ? "#16253A" : "white"
                                            border.width: 2
                                            border.color: Design.colFrameBorder
                                        }

                                        contentItem: Text {
                                            text: if(scanManager.SelectedFuncAScan.paramDAutoGain){
                                                      return qsTr("ACTIF")
                                                  }else{
                                                      return qsTr("INACTIF")
                                                  }
                                            font.family: Fonts.robotoCondensed
                                            font.pointSize: 12
                                            font.bold: true
                                            color: scanManager.SelectedFuncAScan.paramDAutoGain ? "white" : Design.colButtonDarkBackground
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        onClicked: {
                                            scanManager.SelectedFuncAScan.paramDAutoGain = !scanManager.SelectedFuncAScan.paramDAutoGain
                                        }
                                    }
                                }
                                Item { Layout.fillWidth: true }
                            }
                        }
                        Item { Layout.fillWidth: true }
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
