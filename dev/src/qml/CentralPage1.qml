import QtQuick 2.15

CentralPage1Form {
    // CONFIGURATION
    property var comCompA : currentVector.compAScan0().comComponent


    Component.onCompleted: {
        // -- Link
        updateLinks();
    }



    property var linkList : currentVector.getLinkManager().getLinkModel()
    idLinkList.model: linkList
    idLinkList.height: 250
    idLinkList.delegate: ComponentLinkConfig {

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

        Component.onCompleted: {
            idCbLinkComPort.currentIndex = idCbLinkComPort.find(serial_port);
        }

    }





//    Connections {
//        target: currentVector.compAScan0()
//        function on
//            console.log("enable changed")
//        }
//    }

    function updateLinks() {

    }
}
