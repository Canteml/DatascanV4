import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0


RowLayout {
    visible: currentVector.funcLicence().hasReceivedMessage && (currentVector.funcLicence().paramDStatusLicenceP <= 20)
    id: idLicenceWarningBox
    Layout.fillHeight: false
    Layout.fillWidth: true
    Layout.preferredHeight: 70

    Rectangle {
        id: idLicenceWarningBackground
        Layout.fillHeight: true
        Layout.fillWidth: true
        color: currentVector.funcLicence().paramDStatusLicenceP === 0 ? "red" : "orange"
        border.width: 1
        border.color: "black"

        Text {
            id: idLicenceWarningText
            text: currentVector.funcLicence().paramDStatusLicenceP === 0 ?
                      qsTr("Votre licence équipement d'utilisation épuisée ! L'export de CSV est désactivé. Merci de contacter ROBOPLANET pour la renouveller") :
                      qsTr("Votre licence équipement d'utilisation est bientôt épuisée ! Merci de contacter ROBOPLANET pour la renouveller")
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

}
