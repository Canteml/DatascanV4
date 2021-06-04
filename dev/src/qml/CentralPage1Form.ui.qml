import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {
    width: 600
    height: 400
    Layout.fillWidth: true
    Layout.fillHeight: true

    color: "#ffffff"
    property alias idColComponents: idColComponents

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

        ColumnLayout {
            id: idColComponents
            Layout.fillWidth: true
            Layout.leftMargin: 20

            ComponentLinkConfig {
                id: idComponentPosition
                componentId: 0
                componentName: "Position"
            }

            ComponentLinkConfig {
                id: idComponentAScan0
                componentId: 1
                componentName: "ASCAN 1"
            }

            ComponentLinkConfig {
                id: idComponentAScan1
                componentId: 2
                componentName: "ASCAN 2"
            }
        }

        // -- SPACER
        Rectangle {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "teal"
        }
    }
}
