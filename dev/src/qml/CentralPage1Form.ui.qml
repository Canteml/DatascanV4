import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {
    width: 600
    height: 400
    Layout.fillWidth: true
    Layout.fillHeight: true

    color: "#ffffff"
    property alias idLinkList: idLinkList

    ColumnLayout {
        id: idP1MainLayout

        anchors.fill: parent
        anchors.margins: 5

        RowLayout {
            id: idP1EqLy
            Layout.fillWidth: true

            Text {
                id: idLbEq
                text: qsTr("Equipement")
                font.pixelSize: 18
                font.bold: true
                color: "#FF8200"
            }

            Rectangle {
                id: idLineEq
                Layout.fillWidth: true
                Layout.minimumHeight: 3
                color: "#FF8200"
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
            }

            ComboBox {
                id: idCbProduct
                model: ["Altiscan", "Perche", "Buggy", "Panda Vol3"]
            }
        }

        ListView {
            id: idLinkList
            width: 200
            height: 250
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 40
        }

        // -- SPACER
        Item {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
