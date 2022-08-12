import QtQuick 2.15
import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "rp_components"

Rectangle {
    id: idCentralP4

    Layout.fillWidth: true
    Layout.fillHeight: true

    anchors.margins: 5

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 5

        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Licence actuelle")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Utilisation restante : ")
                font.pixelSize: 14
            }

            Text {
                id: idRemainingLicenceLabel
                text: currentVector.funcLicence().paramDStatusLicenceP
                font.pixelSize: 14
            }

            Text {
                text: qsTr("%")
                font.pixelSize: 14
            }

            Item {
                Layout.fillWidth: true
            }

            Connections {
                target: currentVector.funcLicence()
                function onParamDStatusLicencePChanged(status) {
                    idRemainingLicenceLabel.text = status
                }
            }

        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 20

            Button {
                text: qsTr("Actualiser")
                Layout.preferredHeight: 30
                font.pixelSize: 14
                onClicked: currentVector.funcLicence().askForStatusLicence()
            }
        }

        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Demande de nouvelle licence")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Pour demander une nouvelle licence, merci de contacter Roboplanet muni d'une clé d'équipement générée ci-dessous")
                font.pixelSize: 14
            }

        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Clé d'équipement :")
                font.pixelSize: 14
            }

            TextField {
                id: idEquipmentKeyDisplay
                text: ""
                readOnly: true
                selectByMouse: true
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pixelSize: 14
            }

            Button {
                id: idBtGenerateEquipmentKey
                text: qsTr("Générer une clé d'équipement")
                Layout.preferredHeight: 30
                font.pixelSize: 14
                onClicked: currentVector.funcLicence().askForKeyEquipment()
            }

            Button {
                id: idBtCopyEquipmentKey
                text: qsTr("Copier la clé d'équipement")
                Layout.preferredHeight: 30
                enabled: false
                font.pixelSize: 14
                onClicked: clipboard.setText(idEquipmentKeyDisplay.text)
            }

            Connections {
                target: currentVector.funcLicence()
                function onKeyEquipmentChanged(keyEquipment) {
                    idEquipmentKeyDisplay.text = keyEquipment
                    idBtCopyEquipmentKey.enabled = true
                }
            }

        }

        RowLayout {
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Activation d'une nouvelle licence")
            }
        }

        RowLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            Text {
                text: qsTr("Clé de licence :")
                font.pixelSize: 14
            }

            TextField {
                selectByMouse: true
                Layout.fillWidth: true;
                Layout.preferredHeight: 30
                font.pixelSize: 14
            }

            Button {
                text: qsTr("Utiliser la clé")
                Layout.preferredHeight: 30
                font.pixelSize: 14
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }

        BannerBottomNavigate {
            id: idBannerBottom4
            idBtPrev.text : qsTr("SCANNER")
            idBtNext.text : qsTr("AIDE")

            Connections {
                target: idBannerBottom4.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 3;
                }
            }

            Connections {
                target: idBannerBottom4.idBtNext
                function onClicked() {
                    globalManager.guiPage = 5;
                }
            }
        }
    }
}
