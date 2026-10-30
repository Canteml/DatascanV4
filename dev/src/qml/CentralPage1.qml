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

    component CameraRoleCombo: ComboBox {
        id: idCameraRoleCombo
        property var cameraRoleItem

        model: [qsTr("AUCUNE")].concat(typeof modelCameraList !== "undefined" ? modelCameraList : [])
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
            border.width: 1
            border.color: Design.colFrameBorder
        }
        contentItem: Text {
            font.family: "Roboto Condensed"
            text: idCameraRoleCombo.displayText
            font.pixelSize: 12
            font.bold: true
            leftPadding: 6
            color: "#0D2B38"
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        popup: Popup {
            y: idCameraRoleCombo.height
            width: idCameraRoleCombo.width
            implicitHeight: contentItem.implicitHeight
            padding: 1

            contentItem: ListView {
                clip: true
                implicitHeight: contentHeight
                model: idCameraRoleCombo.popup.visible ? idCameraRoleCombo.delegateModel : null
                currentIndex: idCameraRoleCombo.highlightedIndex
                ScrollIndicator.vertical: ScrollIndicator {}
            }

            background: Rectangle {
                color: "#FFFFFF"
                border.color: "#C7CBD1"
                border.width: 1
            }
        }
        delegate: ItemDelegate {
            id: idCameraRoleComboDelegate
            width: idCameraRoleCombo.width
            highlighted: idCameraRoleCombo.highlightedIndex === index
            contentItem: Text {
                font.family: "Roboto Condensed"
                text: idCameraRoleCombo.textAt(index)
                font.pixelSize: 12
                font.bold: true
                color: "#0D2B38"
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                color: idCameraRoleComboDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
            }
        }
    }

    color: "#ffffff"
    //property alias idLinkList: idLinkList
    //property alias idCbProduct: idCbProduct

    ColumnLayout {
        id: idP1MainLayout

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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                            contentItem: Text {
                                font.family: "Roboto Condensed"
                                text: idCbProduct.displayText
                                font.pixelSize: 12
                                font.bold: true
                                leftPadding: 6
                                color: "#0D2B38"
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

                            popup: Popup {
                                y: idCbProduct.height
                                width: idCbProduct.width
                                implicitHeight: contentItem.implicitHeight
                                padding: 1

                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: idCbProduct.popup.visible ? idCbProduct.delegateModel : null
                                    currentIndex: idCbProduct.highlightedIndex
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }

                                background: Rectangle {
                                    color: "#FFFFFF"
                                    border.color: "#C7CBD1"
                                    border.width: 1
                                }
                            }
                            delegate: ItemDelegate {
                                id: idCbProductDelegate
                                width: idCbProduct.width
                                highlighted: idCbProduct.highlightedIndex === index
                                contentItem: Text {
                                    font.family: "Roboto Condensed"
                                    text: idCbProduct.textAt(index)
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: "#0D2B38"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                                background: Rectangle {
                                    color: idCbProductDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                }
                            }

                            onActivated: function(index) {
                                products.setProdId(valueAt(index))
                            }
                            Component.onCompleted: {
                                currentIndex = indexOfValue(products.getCurrentProdId())
                            }
                        }

                        Text {
                            text: qsTr("Baudrate")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 1
                            Layout.column: 0
                        }

                        // Baud rate of the encoder serial link, applied immediately
                        ComboBox {
                            id: idCbEncoderBaudrate
                            Layout.row: 1
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            font.family: Fonts.robotoCondensed
                            model: [
                                { text: "9600",   baud: 9600 },
                                { text: "115200", baud: 115200 }
                            ]
                            textRole: "text"
                            valueRole: "baud"

                            Component.onCompleted: currentIndex = indexOfValue(encoderController.baudRate)

                            Connections {
                                target: encoderController
                                function onBaudRateChanged(value) {
                                    idCbEncoderBaudrate.currentIndex = idCbEncoderBaudrate.indexOfValue(value)
                                }
                            }

                            onActivated: function(index) {
                                encoderController.baudRate = valueAt(index)
                            }

                            background: Rectangle {
                                color: "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                font.family: "Roboto Condensed"
                                text: idCbEncoderBaudrate.displayText
                                font.pixelSize: 12
                                font.bold: true
                                leftPadding: 6
                                color: "#0D2B38"
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

                            popup: Popup {
                                y: idCbEncoderBaudrate.height
                                width: idCbEncoderBaudrate.width
                                implicitHeight: contentItem.implicitHeight
                                padding: 1

                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: idCbEncoderBaudrate.popup.visible ? idCbEncoderBaudrate.delegateModel : null
                                    currentIndex: idCbEncoderBaudrate.highlightedIndex
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }

                                background: Rectangle {
                                    color: "#FFFFFF"
                                    border.color: "#C7CBD1"
                                    border.width: 1
                                }
                            }
                            delegate: ItemDelegate {
                                id: idCbEncoderBaudrateDelegate
                                width: idCbEncoderBaudrate.width
                                highlighted: idCbEncoderBaudrate.highlightedIndex === index
                                contentItem: Text {
                                    font.family: "Roboto Condensed"
                                    text: idCbEncoderBaudrate.textAt(index)
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: "#0D2B38"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                                background: Rectangle {
                                    color: idCbEncoderBaudrateDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                }
                            }
                        }

                        Text {
                            text: qsTr("Port série de secours")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 2
                            Layout.column: 0
                        }

                        ComboBox {
                            id: idCbBackupSerialPort
                            Layout.row: 2
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            font.family: Fonts.robotoCondensed
                            textRole: "text"
                            valueRole: "address"

                            function refreshPorts() {
                                model = serialPortRouter.availablePorts()
                                currentIndex = indexOfValue(serialPortRouter.portName)
                            }

                            Component.onCompleted: refreshPorts()

                            // Rescan ports each time the list is opened (hot-plug)
                            Connections {
                                target: idCbBackupSerialPort.popup
                                function onAboutToShow() {
                                    idCbBackupSerialPort.refreshPorts()
                                }
                            }

                            Connections {
                                target: serialPortRouter
                                function onPortNameChanged() {
                                    idCbBackupSerialPort.refreshPorts()
                                }
                            }

                            onActivated: function(index) {
                                serialPortRouter.portName = valueAt(index)
                            }

                            background: Rectangle {
                                color: "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                font.family: "Roboto Condensed"
                                text: idCbBackupSerialPort.displayText
                                font.pixelSize: 12
                                font.bold: true
                                leftPadding: 6
                                color: "#0D2B38"
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

                            popup: Popup {
                                y: idCbBackupSerialPort.height
                                width: idCbBackupSerialPort.width
                                implicitHeight: contentItem.implicitHeight
                                padding: 1

                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: idCbBackupSerialPort.popup.visible ? idCbBackupSerialPort.delegateModel : null
                                    currentIndex: idCbBackupSerialPort.highlightedIndex
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }

                                background: Rectangle {
                                    color: "#FFFFFF"
                                    border.color: "#C7CBD1"
                                    border.width: 1
                                }
                            }

                            delegate: ItemDelegate {
                                id: idCbBackupSerialPortDelegate
                                width: idCbBackupSerialPort.width
                                highlighted: idCbBackupSerialPort.highlightedIndex === index
                                contentItem: Text {
                                    font.family: "Roboto Condensed"
                                    text: idCbBackupSerialPort.textAt(index)
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: "#0D2B38"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                                background: Rectangle {
                                    color: idCbBackupSerialPortDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
                                }
                            }
                        }

                        Text {
                            text: qsTr("Source du B-Scan")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 3
                            Layout.column: 0
                        }

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

                            function refreshDevices() {
                                model = encoderController.availableDevices()
                                currentIndex = indexOfValue(encoderController.bscanSource)
                            }

                            onActivated: function(index) {
                                encoderController.bscanSource = valueAt(index)
                            }

                            Component.onCompleted: refreshDevices()

                            // Rescan ports each time the list is opened (hot-plug)
                            Connections {
                                target: idCbEncoderDevice.popup
                                function onAboutToShow() {
                                    idCbEncoderDevice.refreshDevices()
                                }
                            }

                            // Network device authenticated / disconnected, source changed
                            Connections {
                                target: encoderController
                                function onAvailableDevicesChanged() {
                                    idCbEncoderDevice.refreshDevices()
                                }
                                function onBscanSourceChanged() {
                                    idCbEncoderDevice.refreshDevices()
                                }
                            }

                            background: Rectangle {
                                color: "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                font.family: "Roboto Condensed"
                                text: idCbEncoderDevice.displayText
                                font.pixelSize: 12
                                font.bold: true
                                leftPadding: 6
                                color: "#0D2B38"
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

                            popup: Popup {
                                y: idCbEncoderDevice.height
                                width: idCbEncoderDevice.width
                                implicitHeight: contentItem.implicitHeight
                                padding: 1

                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: idCbEncoderDevice.popup.visible ? idCbEncoderDevice.delegateModel : null
                                    currentIndex: idCbEncoderDevice.highlightedIndex
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }

                                background: Rectangle {
                                    color: "#FFFFFF"
                                    border.color: "#C7CBD1"
                                    border.width: 1
                                }
                            }
                            delegate: ItemDelegate {
                                id: idCbEncoderDeviceDelegate
                                width: idCbEncoderDevice.width
                                highlighted: idCbEncoderDevice.highlightedIndex === index
                                contentItem: Text {
                                    font.family: "Roboto Condensed"
                                    text: idCbEncoderDevice.textAt(index)
                                    font.pixelSize: 12
                                    font.bold: true
                                    color: "#0D2B38"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                                background: Rectangle {
                                    color: idCbEncoderDeviceDelegate.highlighted ? "#E6EFF3" : "#FFFFFF"
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
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
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
                Layout.preferredHeight: 50
                border.width: 1
                border.color: Design.colFrameBorder

                RowLayout {
                    id: certificatsMenu
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 10
                    anchors.topMargin: 6
                    anchors.bottomMargin: 6
                    spacing: 8


                    property string menuMode: "appareil"

                    Button {
                        id: btAppareilMenu
                        Layout.fillHeight: true
                        Layout.preferredWidth: 140

                        contentItem: Text {
                            text: qsTr("APPAREIL")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: certificatsMenu.menuMode === "appareil" ? "white" : "black"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: certificatsMenu.menuMode === "appareil" ? "#0D2B38" : "white"
                            border.color: Design.colBlackBorder
                            border.width: 3
                        }

                        onClicked: {
                            certificatsMenu.menuMode = "appareil"
                        }
                    }

                    Button {
                        id: btControlleurMenu
                        Layout.fillHeight: true
                        Layout.preferredWidth: 140

                        contentItem: Text {
                            text: qsTr("CONTROLLEUR")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: certificatsMenu.menuMode === "controlleur" ? "white" : "black"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: certificatsMenu.menuMode === "controlleur" ? "#0D2B38" : "white"
                            border.color: Design.colBlackBorder
                            border.width: 3
                        }

                        onClicked: {
                            certificatsMenu.menuMode = "controlleur"
                        }
                    }

                    Button {
                        id: btSuperviseurMenu
                        Layout.fillHeight: true
                        Layout.preferredWidth: 140

                        contentItem: Text {
                            text: qsTr("SUPERVISEUR")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: certificatsMenu.menuMode === "superviseur" ? "white" : "black"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: certificatsMenu.menuMode === "superviseur" ? "#0D2B38" : "white"
                            border.color: Design.colBlackBorder
                            border.width: 3
                        }

                        onClicked: {
                            certificatsMenu.menuMode = "superviseur"
                        }
                    }

                    Button {
                        id: btCapteurMenu
                        Layout.fillHeight: true
                        Layout.preferredWidth: 140

                        contentItem: Text {
                            text: qsTr("CAPTEUR")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            color: certificatsMenu.menuMode === "capteur" ? "white" : "black"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: certificatsMenu.menuMode === "capteur" ? "#0D2B38" : "white"
                            border.color: Design.colBlackBorder
                            border.width: 3
                        }

                        onClicked: {
                            certificatsMenu.menuMode = "capteur"
                        }
                    }
                }
            }

            Rectangle {
                id: rectCertificat
                color: "white"
                Layout.fillWidth: true
                Layout.leftMargin: 5
                Layout.rightMargin: 5
                Layout.preferredHeight: 105
                border.width: 1
                border.color: Design.colFrameBorder

                ColumnLayout {
                    id: certificatColumn
                    anchors.fill: parent
                    anchors.margins: 15
                    anchors.bottomMargin: 20
                    // gap grid/warning comes from the grids' rowSpacing before their filler row
                    spacing: 0

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 4
                        columnSpacing: 15
                        rowSpacing: 15

                        visible: certificatsMenu.menuMode === "appareil"

                        Text {
                            text: qsTr("Appareil UT")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.equipmentNom
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Numéro de série")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.equipmentNumero
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Date d'expiration du certificat")
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
                                border.width: 1
                                border.color:
                                    if(examManager.dateValide){
                                        Design.colFrameBorder
                                    }else{
                                        "#8E1B1B"
                                    }
                            }
                        }
                        Item {
                            Layout.row: 2
                            Layout.column: 0
                            Layout.columnSpan: 4
                            Layout.fillHeight: true
                        }

                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 4
                        columnSpacing: 15
                        rowSpacing: 15

                        visible: certificatsMenu.menuMode === "controlleur"

                        Text {
                            text: qsTr("Nom du UT3")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.inspecteurNom
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Numéro du UT3")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.inspecteurNumero
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Date d'expiration certificat UT3")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.inspecteurDate
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 1
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            onTextChanged: {examManager.inspecteurDate = text}
                            Keys.onReturnPressed: {
                                console.log("Captured !")
                            }
                            background: Rectangle {
                                color:
                                    if(examManager.inspecteurValide){
                                        "white"
                                    }else{
                                        "#F7E6E6"
                                    }
                                border.width: 1
                                border.color:
                                    if(examManager.inspecteurValide){
                                        Design.colFrameBorder
                                    }else{
                                        "#8E1B1B"
                                    }
                            }
                        }
                        Item {
                            Layout.row: 2
                            Layout.column: 0
                            Layout.columnSpan: 4
                            Layout.fillHeight: true
                        }

                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 4
                        columnSpacing: 15
                        rowSpacing: 15

                        visible: certificatsMenu.menuMode === "superviseur"

                        Text {
                            text: qsTr("Nom du superviseur")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.supervisorName
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Numéro du superviseur")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.supervisorNumber
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Date de contrôle")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 1
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.supervisorDate
                            font.family: Fonts.robotoCondensed
                            selectByMouse: true
                            color: "black"
                            Layout.row: 1
                            Layout.column: 1
                            Layout.fillWidth: true
                            Layout.preferredWidth: 220
                            Layout.preferredHeight: 30
                            Keys.onReturnPressed: {
                                console.log("Captured !")
                            }
                            background: Rectangle {
                                color: "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }
                        Item {
                            Layout.row: 2
                            Layout.column: 0
                            Layout.columnSpan: 4
                            Layout.fillHeight: true
                        }
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 4
                        columnSpacing: 15
                        rowSpacing: 15

                        visible: certificatsMenu.menuMode === "capteur"

                        Text {
                            text: qsTr("Numéro de la sonde")
                            font.pixelSize: 14
                            font.family: Fonts.robotoCondensed
                            Layout.row: 0
                            Layout.column: 0
                        }

                        TextField {
                            text: examManager.sondeNumero
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }

                        Text {
                            text: qsTr("Nom du contrôleur")
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 14
                            Layout.row: 0
                            Layout.column: 2
                        }

                        TextField {
                            text: examManager.controlleurNom
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
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }
                        }
                        Item {
                            Layout.row: 1
                            Layout.column: 0
                            Layout.columnSpan: 4
                            Layout.fillHeight: true
                        }
                    }
                }
            }
        }

        // -- SPACER
        Item {
            // Spacer
            Layout.fillHeight: true
        }
        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.text : qsTr("PROJET")
            idBtNext.text : qsTr("SCANNER")

            Connections {
                target: idBannerBottom2.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 0;
                }
            }

            Connections {
                target: idBannerBottom2.idBtNext
                function onClicked() {
                    globalManager.guiPage = 3;
                }
            }
        }
    }
}
