import QtQuick 2.4
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.11
import Design 1.0
import QtGraphicalEffects 1.0
import "../rp_components"

Rectangle {

    color: "#FFFFFF"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true


        }

        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 0.3 * parent.height

            TextArea {
                id: idGlobalCommentField
                font.pointSize: 12

                background: Rectangle {
                    border.width: 1
                    color: "white"//"#E5E5E5"
                }

                Text {
                    visible: (!parent.text && !parent.activeFocus)
                    anchors.fill: parent
                    text: qsTr("Commentaire global sur l'acquisition")
                    font.italic: true
                    color: "#555555"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 12
                }
            }
        }

        ScrollView {
            id: idCellsAnnotationsField
            Layout.fillWidth: true
            Layout.preferredHeight: 0.3 * parent.height

            TextArea {
                font.pointSize: 12

                background: Rectangle {
                    border.width: 1
                    color: "white"//"#E5E5E5"
                }

                Text {
                    visible: (!parent.text && !parent.activeFocus)
                    anchors.fill: parent
                    text: qsTr("A5 : Mesure impossible...")
                    font.italic: true
                    color: "#555555"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 12
                }
            }
        }

    }

}
