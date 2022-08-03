import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0


RowLayout {
    visible: false
    id: idLicenceWarningBox
    Layout.fillHeight: false
    Layout.fillWidth: true
    Layout.preferredHeight: 70

    Rectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true
        color: "red"
        border.width: 1
        border.color: "black"

        Text {
            id: idLicenceWarningText
            text: qsTr("Votre licence équipement d'utilisation est bientôt épuisée ! Merci de contacter ROBOPLANET pour la renouveller")
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            color: "white"
        }
    }

    Connections {
        target: licenceManager
        function onLicenceValidityChanged(validity) {
            idLicenceWarningBox.visible = !validity
        }
    }

}
