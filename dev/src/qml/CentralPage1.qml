import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtMultimedia
import "rp_components"
import Design 1.0
import Fonts 1.0

import Camera 1.0

Rectangle {
    width: 600
    height: 400
    Layout.fillWidth: true
    Layout.fillHeight: true

    Connections {
        target: globalManager
        function onEvGuiPageChanged(page) {
            if (page !== 1) {
                cameraManager.setPreviewCameraIndex(-1)
            } else {
                // Riavvia il preview quando torni sulla pagina
                if (modelCameraList !== undefined && modelCameraList.length > 0) {
                    cameraManager.setPreviewCameraIndex(0)
                }
            }
        }
    }

    // ComboBox assigning a camera to a role as soon as it is selected
    // index 0 = no camera, index n = camera n-1 of modelCameraList
    component CameraRoleCombo: ComboBox {
        id: idCameraRoleCombo
        property var cameraRoleItem

        model: [qsTr("Aucune")].concat(typeof modelCameraList !== "undefined" ? modelCameraList : [])
        enabled: cameraRoleItem.isEnabled
        font.family: Fonts.robotoCondensed

        function syncIndex() {
            currentIndex = cameraRoleItem.cameraIndex + 1
        }

        onModelChanged: syncIndex()
        Component.onCompleted: syncIndex()

        onActivated: function(index) {
            cameraManager.assignCameraToRole(cameraRoleItem.getRole(), index - 1)
        }

        Connections {
            target: idCameraRoleCombo.cameraRoleItem
            function onEvCameraIndexChanged() {
                idCameraRoleCombo.syncIndex()
            }
        }

        background: Rectangle {
            color: idCameraRoleCombo.enabled ? "white" : "#EEEEEE"
            border.width: 2
            border.color: Design.colFrameBorder
        }
    }

    color: "#ffffff"
    //property alias idLinkList: idLinkList
    //property alias idCbProduct: idCbProduct

    ColumnLayout {
        id: idP1MainLayout

        anchors.fill: parent
        anchors.margins: 5

        /*


        // -- EQUIPEMENT
        RowLayout {
            id: idP1EqLy
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Equipement")
            }
        }

        RowLayout {
            id: idRowProduct
            Layout.fillWidth: true
            Layout.leftMargin: 20

            Text {
                id: lbProduct
                text: qsTr("Appareil :")
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
                Layout.preferredWidth: 100

                Layout.preferredHeight: idRowProduct.height

            }

            ComboBox {
                id: idCbProduct
                Layout.preferredHeight: 30
                Layout.preferredWidth: 200
                model: products.enabledProducts //[qsTr("UT POLE"), qsTr("ALTISCAN"), qsTr("ROVER"), qsTr("ROVER IP68"), //qsTr("ROVER CLEANER"),
                    //qsTr("PANDA Vol.3"), qsTr("MULTISCAN")]
                property var ready : false

                onCurrentIndexChanged: {
                    if( ready ) {
                        products.setProdIndex(currentIndex)
                    }
                }
                Component.onCompleted: {
                    currentIndex = products.getCurrentIndex()
                    ready = true;
                }
            }
        }

        ListView {
            // lsit of devices
            id: idlvListDevices
            Layout.fillWidth: true
            Layout.leftMargin: 130
            model: modelDeviceList
            delegate: Rectangle {
                height: 40
                Text {
                     text: modelData
                     font.pixelSize: 14
                }
            }

            Connections {
                // Change the size of the listview function to the number of items
                target: currentVector.getDeviceManager()
                function onEvDeviceListChanged(p_value) {
                    //console.log("value " + p_value);
                    idlvListDevices.Layout.preferredHeight = (p_value) * 35;
                    idlvListDevices.height = (p_value) * 35;
                }
            }
        }

        // -- CAMERA
        RowLayout {
            id: idP1CamLy
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Caméra")
            }
        }

        RowLayout {
            id: idRwCam
            Layout.fillWidth: true
            Layout.leftMargin: 20
            ColumnLayout {
                id: idClCam
                Layout.alignment: Qt.AlignTop
                // One row by camera role
                CameraParam {
                    cameraRoleText : qsTr("Alarme :")
                    cameraRoleItem : cameraManager.camAlarm()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 1 :")
                    cameraRoleItem : cameraManager.camSensor1()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 2 :")
                    cameraRoleItem : cameraManager.camSensor2()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 3 :")
                    cameraRoleItem : cameraManager.camSensor3()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 4 :")
                    cameraRoleItem : cameraManager.camSensor4()
                }
                CameraParam {
                    cameraRoleText : qsTr("Avant :")
                    cameraRoleItem : cameraManager.camFront()
                }
                CameraParam {
                    cameraRoleText : qsTr("Arrière :")
                    cameraRoleItem : cameraManager.camRear()
                }
            }
            Rectangle {
                // Vertical orange bar
                Layout.preferredWidth: 3
                Layout.preferredHeight: idRwCam.height
                width : 3
                color: Design.colFromSection
            }

            ColumnLayout {
                RowLayout {
                    id: idRwCamPreview
                    Layout.fillWidth: true

                    Rectangle {
                        id: idFrameCamPreview
                        border.color: Design.colFrameBorder
                        border.width: 1
                        Layout.preferredHeight: 200
                        Layout.preferredWidth: 420
                        visible: true

                        VideoOutput {
                            id: idVideo
                            objectName: "previewVideoOutput"
                            anchors.fill: parent
                            anchors.margins: 1

                            Component.onCompleted: {
                                cameraManager.setPreviewVideoSink(idVideo)
                            }
                        }

                        Image {
                            anchors.fill: parent
                            anchors.margins: 1
                            source: "qrc:///img/icons/no_camera.png"
                            fillMode: Image.PreserveAspectFit
                            visible: !idVideo.hasVideo && idCbSelectCameraPreview.currentIndex === 0
                        }

                        Component.onCompleted: {
                            cameraManager.setPreviewVideoSink(idVideo)
                        }

                        Connections {
                            target: cameraManager
                            function onIsPreviewCameraIpChanged(isIp) {
                                cameraManager.setPreviewVideoSink(idVideo)
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.maximumWidth: idFrameCamPreview.width
                    Text {
                        text: qsTr("Caméra : ")
                        font.pixelSize: 14
                    }

                    ComboBox {
                        id: idCbSelectCameraPreview
                        model: modelCameraList
                        Layout.fillWidth: true

                        onActivated: {
                            cameraManager.setPreviewCameraIndex(currentIndex)
                        }
                    }

                }

                Connections {
                    target: cameraManager
                    function onEvCameraListChanged(size) {
                        if (size > 0 && cameraManager.getPreviewCameraIndex() === -1) {
                            idCbSelectCameraPreview.currentIndex = 0
                            cameraManager.setPreviewCameraIndex(0)
                        }
                    }
                }

            }
            // -- SPACER
            Item {
                // Spacer
                Layout.fillWidth: true
            }

            Connections {
                target : cameraManager
                function onPreviewCameraIndexChanged(p_index) {
                    if( idCbSelectCameraPreview.currentIndex !== p_index ) {
                        idCbSelectCameraPreview.currentIndex = p_index;
                    }
                }
            }
        }

        ColumnLayout {
            id: idP1Inclo
            Layout.fillWidth: true
            visible: currentVector.funcInertialSensor().isEnabled && currentVector.funcInertialSensor().hasReceivedMessage

            FormSection {
                text: qsTr("Inclinomètre")
            }

            RowLayout {
                Layout.fillWidth: true
                Button {
                    icon.source: "qrc:///img/icons/on_ground.png"
                    icon.width: 50
                    icon.height: 50
                    icon.color: "transparent"

                    antialiasing: true
                    enabled: currentVector.funcInertialSensor().paramLHorizontal == 1
                    onClicked: currentVector.funcInertialSensor().paramLHorizontal = 0
                    background: Rectangle {
                        color: parent.enabled ? "white" : "#DDDDDD"
                        border.color: Design.colButtonBorder
                        radius: 10
                    }
                }
                Button {
                    icon.source: "qrc:///img/icons/on_wall.png"
                    icon.width: 50
                    icon.height: 50
                    icon.color: "transparent"

                    antialiasing: true
                    enabled: currentVector.funcInertialSensor().paramLHorizontal == 0
                    onClicked: currentVector.funcInertialSensor().paramLHorizontal = 1
                    background: Rectangle {
                        color: parent.enabled ? "white" : "#DDDDDD"
                        border.color: Design.colButtonBorder
                        radius: 10
                    }
                }
            }
        }
*/

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
                            text: qsTr("PORTEUR ET LIAISON")
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
                    height: 250
                    border.width: 1
                    border.color: Design.colFrameBorder

                    GridLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 15
                        columns: 2
                        columnSpacing: 15
                        rowSpacing: 15

                        Text {
                            text: qsTr("Porteur")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        ComboBox {
                            id: idCbProduct
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 1
                            // Full product list, always shown (prodId = ProtMsg::PROD_ID)
                            model: [
                                { text: qsTr("UT POLE"),     prodId: 1 },
                                { text: qsTr("ALTISCAN"),    prodId: 2 },
                                { text: qsTr("ROVER"),       prodId: 3 },
                                { text: qsTr("ROVER IP68"),  prodId: 4 },
                                //{ text: qsTr("ROVER CLEANER"), prodId: 5 },
                                { text: qsTr("PANDA Vol.3"), prodId: 7 },
                                { text: qsTr("MULTISCAN"),   prodId: 8 }
                            ]
                            textRole: "text"
                            valueRole: "prodId"

                            background: Rectangle {
                                color: "white"
                                border.width: 2
                                border.color: Design.colFrameBorder
                            }

                            onActivated: function(index) {
                                products.setProdId(valueAt(index))
                            }
                            Component.onCompleted: {
                                currentIndex = indexOfValue(products.getCurrentProdId())
                            }
                        }

                        /*
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
                        }*/

                        Text {
                            text: qsTr("Liaison")
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 0
                            font.family: Fonts.robotoCondensed
                        }

                        TextField {
                            text: examManager.structName
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 1
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
                            text: qsTr("Port série de secours")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 2
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.structName
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 2
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
                            text: qsTr("Adresse IP de l'encodeur")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 3
                            Layout.column: 0
                        }

                        // List of USB devices an encoder can be connected to.
                        // Selection has no effect yet.
                        ComboBox {
                            id: idCbEncoderDevice
                            Layout.row: 3
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            font.family: Fonts.robotoCondensed
                            textRole: "text"
                            valueRole: "address"
                            displayText: count > 0 ? currentText : qsTr("Aucun appareil détecté")

                            function refreshDevices() {
                                const previous = currentValue
                                model = encoderController.availableDevices()
                                const idx = indexOfValue(previous)
                                currentIndex = idx >= 0 ? idx : (count > 0 ? 0 : -1)
                            }

                            Component.onCompleted: refreshDevices()

                            // Rescan ports each time the list is opened (hot-plug)
                            Connections {
                                target: idCbEncoderDevice.popup
                                function onAboutToShow() {
                                    idCbEncoderDevice.refreshDevices()
                                }
                            }

                            // Network device authenticated / disconnected
                            Connections {
                                target: encoderController
                                function onAvailableDevicesChanged() {
                                    idCbEncoderDevice.refreshDevices()
                                }
                            }

                            background: Rectangle {
                                color: "white"
                                border.width: 2
                                border.color: Design.colFrameBorder
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
                            text: qsTr("CAMÉRAS IP")
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
                    height: 250
                    border.width: 1
                    border.color: Design.colFrameBorder

                    GridLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 15
                        columns: 3
                        columnSpacing: 15
                        rowSpacing: 15

                        Text {
                            text: qsTr("Avant")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 0
                        }

                        CameraRoleCombo {
                            cameraRoleItem: cameraManager.camFront()
                            Layout.row: 0
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                        }

                        Rectangle {
                            Layout.row: 0
                            Layout.column: 2
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            radius: 10
                            border.color: "black"
                            color: cameraManager.camFront().cameraIndex >= 0 ? Design.colGreen : "white"
                        }

                        Text {
                            text: qsTr("Arrière")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 0
                        }

                        CameraRoleCombo {
                            cameraRoleItem: cameraManager.camRear()
                            Layout.row: 1
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                        }

                        Rectangle {
                            Layout.row: 1
                            Layout.column: 2
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            radius: 10
                            border.color: "black"
                            color: cameraManager.camRear().cameraIndex >= 0 ? Design.colGreen : "white"
                        }

                        Text {
                            text: qsTr("Alarm")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 2
                            Layout.column: 0
                        }

                        CameraRoleCombo {
                            cameraRoleItem: cameraManager.camAlarm()
                            Layout.row: 2
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                        }

                        Rectangle {
                            Layout.row: 2
                            Layout.column: 2
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            radius: 10
                            border.color: "black"
                            color: cameraManager.camAlarm().cameraIndex >= 0 ? Design.colGreen : "white"
                        }

                        Item {
                            Layout.row: 3
                            Layout.column: 0
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        Button {
                            id: idBtVisualizer
                            Layout.fillWidth: true
                            Layout.preferredHeight: 60
                            Layout.row: 4
                            Layout.column: 0

                            background: Rectangle {
                                color: "white"
                                border.width: 2
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                text: "VISUALISER"
                                font.family: Fonts.robotoCondensed
                                font.pixelSize: 14
                                font.bold: true
                                color: Design.colMainInk
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
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
                        text: qsTr("CERTIFICATS ET TRAÇABILITÉ")
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
                id: rectCertificat
                color: "white"
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: 105
                border.width: 1
                border.color: Design.colFrameBorder

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 15

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 4
                        columnSpacing: 15
                        rowSpacing: 15

                        Text {
                            text: qsTr("Appareil UT · n° de série")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.equipmentNom + " · " + examManager.equipmentNumero
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 0
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
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
                            text: qsTr("Sonde · n° de série")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.sondeNumero
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 0
                            Layout.column: 3
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
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
                            text: qsTr("Certificat d'étalonnage sonde")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.equipmentDate
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 1
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            onTextChanged: {examManager.equipmentDate = text}
                            Keys.onReturnPressed: {
                                console.log("Captured !")
                            }
                            background: Rectangle {
                                color:
                                    if(examManager.dateValide){
                                        "white"
                                    }else{
                                        "#F7E6E6"
                                    }
                                border.width: 2
                                border.color:
                                    if(examManager.dateValide){
                                        Design.colFrameBorder
                                    }else{
                                        "#8E1B1B"
                                    }
                            }
                        }

                        Text {
                            text: qsTr("Certificat d'étalonnage bloc")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.sondeAnee
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 1
                            Layout.column: 3
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            onTextChanged: {examManager.sondeAnee = text}
                            Keys.onReturnPressed: {
                                console.log("Captured !")
                            }
                            background: Rectangle {
                                color:
                                    if(examManager.sondeDateValide){
                                        "white"
                                    }else{
                                        "#F7E6E6"
                                    }
                                border.width: 2
                                border.color:
                                    if(examManager.sondeDateValide){
                                        Design.colFrameBorder
                                    }else{
                                        "#8E1B1B"
                                    }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        Rectangle {
                            id: rectCertificatWarning
                            visible: !examManager.dateValide || !examManager.sondeDateValide
                            color: "#F7E6E6"
                            border.width: 3
                            border.color: "#8E1B1B"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100

                            ColumnLayout{
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                Text {
                                    text: (!examManager.dateValide && !examManager.sondeDateValide)
                                          ? qsTr("2 certificats échus")
                                          : qsTr("1 certificat échu")
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 18
                                    font.bold: true
                                    color: "#8E1B1B"
                                }
                                Text {
                                    visible: !examManager.dateValide
                                    text: qsTr("Certificat d'étalonnage de la sonde expiré")
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#8E1B1B"
                                }
                                Text {
                                    visible: !examManager.sondeDateValide
                                    text: qsTr("Certificat d'étalonnage bloc expiré")
                                    font.family: Fonts.robotoCondensed
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#8E1B1B"
                                }

                            }
                        }

                        // Keeps the button on the right when the warning is hidden
                        Item {
                            visible: !rectCertificatWarning.visible
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }


//        ListView {
//            id: idLinkList
//            width: 200
//            height: 250
//            Layout.fillHeight: true
//            Layout.fillWidth: true
//            Layout.leftMargin: 40


//            property var linkList : currentVector.getLinkManager().getLinkModel()
//            model: linkList
//            delegate: DeviceLinkConfig {

//                idCbLinkType.currentIndex: link_type-1  // enum 0 = unknow
//                idCbLinkType.visible: false // not available for now
//                idCbLinkComPort.model: serial_port_list
//                idCbLinkComPort.enabled: link_state === 0 /* disconnected */ || link_state === 3 /* disconnecting */ || link_state === 4 /* disconnected */
//                idBtLinkConnect.text : link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/? qsTr("Déconnecter") : qsTr("Connecter")
//                linkName : link_name
//                linkState: link_state

//                Connections {
//                    target : idBtLinkConnect
//                    function onPressed() {
//                        if( link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/ ) {
//                            currentVector.getLinkManager().disconnectLink(link_name);
//                        } else {
//                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
//                        }
//                    }

//                }

//                Connections {
//                    target : idCbLinkComPort
//                    function onActivated() {
//                        if( idCbLinkComPort.currentText !== currentVector.getLinkManager().getComPort(link_name) ) {
//                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
//                        }
//                    }
//                }

//                Connections {
//                    target : idCbProduct
//                    function onCurrentIndexChanged() {
//                        if( currentVector.prodId !== idCbProduct.currentIndex+1 ) {
//                            currentVector.prodId = idCbProduct.currentIndex+1; // combo doesn't start to "undef"
//                        }
//                    }
//                }

//                Component.onCompleted: {
//                    idCbLinkComPort.currentIndex = idCbLinkComPort.find(serial_port);
//                    idCbProduct.currentIndex = currentVector.prodId-1
//                }

//            }
//        }

        // -- SPACER
        Item {
            // Spacer
            Layout.fillHeight: true
        }
        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.text : qsTr("PROJET")
            idBtNext.text : qsTr("SUPERVISER")

            Connections {
                target: idBannerBottom2.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 0;
                }
            }

            Connections {
                target: idBannerBottom2.idBtNext
                function onClicked() {
                    globalManager.guiPage = 2;
                }
            }
        }
    }
}
