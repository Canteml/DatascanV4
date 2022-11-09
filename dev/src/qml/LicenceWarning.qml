import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

ColumnLayout {
    //visible: currentVector.funcLicence().hasReceivedMessage && (currentVector.funcLicence().paramDStatusLicenceP <= 20)
    id: idWarningBox
    Layout.fillHeight: false
    Layout.fillWidth: true
    spacing: 0
    //Layout.preferredHeight: 70

    Rectangle {
        id: idAlertLevel0
        Layout.preferredHeight: 70
        Layout.fillWidth: true
        color: "green"
        border.width: 1
        border.color: "black"
        visible: false

        Text {
            id: idTextAlert0
            text: ""
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            color: "white"
        }
    }

    Rectangle {
        id: idAlertLevel1
        Layout.preferredHeight: 70
        Layout.fillWidth: true
        color: "orange"
        border.width: 1
        border.color: "black"
        visible: false

        Text {
            id: idTextAlert1
            text: ""
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            color: "white"
        }
    }

    Rectangle {
        id: idAlertLevel2
        Layout.preferredHeight: 70
        Layout.fillWidth: true
        color: "red"
        border.width: 1
        border.color: "black"
        visible: false

        Text {
            id: idTextAlert2
            text: ""
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            color: "white"
        }
    }

    Rectangle {
        id: idLicenceWarningBackground
        visible: currentVector.funcLicence().hasReceivedMessage && (currentVector.funcLicence().paramDStatusLicenceP <= 20)
        Layout.preferredHeight: 70
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

    Connections {
        target: currentVector
        function onAlertChanged(alertLevel) {
            switch(alertLevel) {
            case 0:
                idAlertLevel0.visible = currentVector.getAlertMsg(alertLevel).length > 0
                idTextAlert0.text = currentVector.getAlertMsg(alertLevel)
                break
            case 1:
                idAlertLevel1.visible = currentVector.getAlertMsg(alertLevel).length > 0
                idTextAlert1.text = currentVector.getAlertMsg(alertLevel)
                break
            case 2:
                idAlertLevel2.visible = currentVector.getAlertMsg(alertLevel).length > 0
                idTextAlert2.text = currentVector.getAlertMsg(alertLevel)
                break
            default:
                break
            }
        }
    }

}
