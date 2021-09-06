import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {

    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        id: idP2MainLayout

        anchors.fill: parent
        anchors.margins: 5


        // -- SPACER
        Item {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }


        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.text : qsTr("PREPARER")
            idBtNext.text : qsTr("SCANNER")

            Connections {
                target: idBannerBottom2.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 1;
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
