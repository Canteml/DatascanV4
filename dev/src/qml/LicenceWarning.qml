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
        id: idLicenceWarningBackground
        Layout.fillHeight: true
        Layout.fillWidth: true
        color: "red"
        border.width: 1
        border.color: "black"

        Text {
            id: idLicenceWarningText
            text: qsTr("Votre licence équipement d'utilisation est bientôt épuisée ! Merci de contacter ROBOPLANET pour la renouveller")
            visible: currentVector.funcLicence().paramDStatusLicenceP <= 20
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
        target: currentVector.funcLicence()
        function onParamDStatusLicencePChanged(status) {
            if(status === 0) {
                idLicenceWarningBox.visible = true
                idLicenceWarningText.text = qsTr("Votre licence équipement d'utilisation épuisée ! L'export de CSV est désactivé. Merci de contacter ROBOPLANET pour la renouveller")
                idLicenceWarningBackground.color = "red"
                return
            }
            if(status <= 20) {
                idLicenceWarningBox.visible = true
                idLicenceWarningText.text = qsTr("Votre licence équipement d'utilisation est bientôt épuisée ! Merci de contacter ROBOPLANET pour la renouveller")
                idLicenceWarningBackground.color = "orange"
                return
            }
            idLicenceWarningBox.visible = false
        }
    }

}
