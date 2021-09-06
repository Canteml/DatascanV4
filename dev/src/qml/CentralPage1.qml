import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "rp_components"

Rectangle {
    width: 600
    height: 400
    Layout.fillWidth: true
    Layout.fillHeight: true

    color: "#ffffff"
    property alias idLinkList: idLinkList
    property alias idCbProduct: idCbProduct

    ColumnLayout {
        id: idP1MainLayout

        anchors.fill: parent
        anchors.margins: 5

        RowLayout {
            id: idP1EqLy
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Equipement")
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
                Layout.preferredHeight: 30
                model: ["Altiscan", "Perche", "Rover", "Panda Vol1", "Panda Vol3"]
            }
        }

        ListView {
            id: idLinkList
            width: 200
            height: 250
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 40


            property var linkList : currentVector.getLinkManager().getLinkModel()
            model: linkList
            delegate: DeviceLinkConfig {

                idCbLinkType.currentIndex: link_type-1  // enum 0 = unknow
                idCbLinkType.visible: false // not available for now
                idCbLinkComPort.model: serial_port_list
                idCbLinkComPort.enabled: link_state === 0 /* disconnected */ || link_state === 3 /* disconnecting */ || link_state === 4 /* disconnected */
                idBtLinkConnect.text : link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/? qsTr("Déconnecter") : qsTr("Connecter")
                linkName : link_name
                linkState: link_state

                Connections {
                    target : idBtLinkConnect
                    function onPressed() {
                        if( link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/ ) {
                            currentVector.getLinkManager().disconnectLink(link_name);
                        } else {
                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
                        }
                    }

                }

                Connections {
                    target : idCbLinkComPort
                    function onActivated() {
                        if( idCbLinkComPort.currentText !== currentVector.getLinkManager().getComPort(link_name) ) {
                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
                        }
                    }
                }

                Connections {
                    target : idCbProduct
                    function onCurrentIndexChanged() {
                        if( currentVector.prodId !== idCbProduct.currentIndex+1 ) {
                            currentVector.prodId = idCbProduct.currentIndex+1; // combo doesn't start to "undef"
                        }
                    }
                }

                Component.onCompleted: {
                    idCbLinkComPort.currentIndex = idCbLinkComPort.find(serial_port);
                    idCbProduct.currentIndex = currentVector.prodId-1
                }

            }
        }

        // -- SPACER
        Item {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.text : qsTr("PROJET")
            idBtNext.text : qsTr("SUPERVISER")

            Connections {
                target: idBannerBottom2.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 0;
                }
            }

            Connections {
                target: idBannerBottom2.idBtNext
                function onClicked() {
                    globalManager.guiPage = 2;
                }
            }
        }
    }
}
