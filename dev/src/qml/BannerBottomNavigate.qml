import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

RowLayout {

    property alias idBtPrev: idBtPrev
    property alias idBtNext: idBtNext

    Layout.preferredHeight: 60
    Layout.fillHeight: true
    Layout.margins: 5

    Rectangle {
        color: "#D5DEE3"
        Layout.fillWidth: true
        Layout.preferredHeight: 56
        Layout.leftMargin: 10
        Layout.rightMargin: 10
        border.width: 1
        border.color: Design.colFrameBorder

        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 15

            Button {
                id: idBtPrev
                text: qsTr("PROJET")
                Layout.preferredHeight: 40
                Layout.preferredWidth: 160

                background: Rectangle {
                    color: "white"
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: "◄  " + idBtPrev.text
                    font.family: Fonts.robotoCondensed
                    font.pixelSize: 14
                    font.bold: true
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item {
                // Spacer
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Button {
                id: idBtNext
                text: qsTr("SCANNER")
                Layout.preferredHeight: 40
                Layout.preferredWidth: 160

                background: Rectangle {
                    color: Design.colButtonDarkBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: idBtNext.text + "  ►"
                    font.family: Fonts.robotoCondensed
                    font.pixelSize: 14
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
