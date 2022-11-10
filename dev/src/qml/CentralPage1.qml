import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtMultimedia 5.15
import "rp_components"
import Design 1.0

import VLCQt 1.1

Rectangle {
    width: 600
    height: 400
    Layout.fillWidth: true
    Layout.fillHeight: true

    color: "#ffffff"
    //property alias idLinkList: idLinkList
    property alias idCbProduct: idCbProduct

    ColumnLayout {
        id: idP1MainLayout

        anchors.fill: parent
        anchors.margins: 5

        // -- EQUIPEMENT
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
                verticalAlignment: Text.AlignVCenter
                Layout.preferredWidth: 100

                Layout.preferredHeight: idRowProduct.height

            }

            ComboBox {
                id: idCbProduct
                Layout.preferredHeight: 30
                Layout.preferredWidth: 200
                model: products.enabledProducts //[qsTr("UT POLE"), qsTr("ALTISCAN"), qsTr("ROVER"), qsTr("ROVER IP68"), //qsTr("ROVER CLEANER"),
                    //qsTr("PANDA Vol.3"), qsTr("MULTISCAN")]
                property var ready : false

                onCurrentIndexChanged: {
                    if( ready ) {
                        products.setProdIndex(currentIndex)
                    }
                }
                Component.onCompleted: {
                    currentIndex = products.getCurrentIndex()
                    ready = true;
                }
            }
        }

        ListView {
            // lsit of devices
            id: idlvListDevices
            Layout.fillWidth: true
            Layout.leftMargin: 130
            model: modelDeviceList
            delegate: Rectangle {
                height: 40
                Text {
                     text: modelData
                     font.pixelSize: 14
                }
            }

            Connections {
                // Change the size of the listview function to the number of items
                target: currentVector.getDeviceManager()
                function onEvDeviceListChanged(p_value) {
                    //console.log("value " + p_value);
                    idlvListDevices.Layout.preferredHeight = (p_value) * 35;
                    idlvListDevices.height = (p_value) * 35;
                }
            }
        }

        // -- CAMERA
        RowLayout {
            id: idP1CamLy
            Layout.fillWidth: true

            FormSection {
                text: qsTr("Caméra")
            }
        }

        RowLayout {
            id: idRwCam
            Layout.fillWidth: true
            Layout.leftMargin: 20
            ColumnLayout {
                id: idClCam
                Layout.alignment: Qt.AlignTop
                // One row by camera role
                CameraParam {
                    cameraRoleText : qsTr("Alarme :")
                    cameraRoleItem : cameraManager.camAlarm()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 1 :")
                    cameraRoleItem : cameraManager.camSensor1()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 2 :")
                    cameraRoleItem : cameraManager.camSensor2()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 3 :")
                    cameraRoleItem : cameraManager.camSensor3()
                }
                CameraParam {
                    cameraRoleText : qsTr("Capteur 4 :")
                    cameraRoleItem : cameraManager.camSensor4()
                }
                CameraParam {
                    cameraRoleText : qsTr("Avant :")
                    cameraRoleItem : cameraManager.camFront()
                }
                CameraParam {
                    cameraRoleText : qsTr("Arrière :")
                    cameraRoleItem : cameraManager.camRear()
                }
            }
            Rectangle {
                // Vertical orange bar
                Layout.preferredWidth: 3
                Layout.preferredHeight: idRwCam.height
                width : 3
                color: Design.colFromSection
            }

            ColumnLayout {
                Camera {
                    id: idCameraPreview
                    objectName: "cameraPreview"

                    Component.onCompleted: idCameraPreview.stop()
                }

                RowLayout {
                    id: idRwCamPreview
                    Layout.fillWidth: true

                    Rectangle {
                        id: idFrameCamPreview
                        border.color: Design.colFrameBorder
                        border.width: 1
                        Layout.preferredHeight: idVideo.height+2
                        Layout.preferredWidth: idVideo.width+2
                        visible: !cameraManager.isPreviewCameraIp
                        VideoOutput {
                            id: idVideo
                            // Keept aspect ration of camera frame
                            height: 200
                            width: sourceRect.width * 200 / sourceRect.height

                            // Frame around the camera
                            x:1
                            y:1

                            //source: cameraManager.previewCamera
                            source: idCameraPreview
                        }
                    }

                    Rectangle {
                        border.color: Design.colFrameBorder
                        border.width: 1
                        Layout.preferredHeight: 200
                        Layout.preferredWidth: 420
                        visible: cameraManager.isPreviewCameraIp
                        VlcVideoOutput {
                            anchors.fill: parent
                            source: cameraManager.getPreviewCameraSource()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Layout.maximumWidth: idFrameCamPreview.width
                    Text {
                        text: qsTr("Camera : ")
                        font.pixelSize: 14
                    }

                    ComboBox {
                        id: idCbSelectCameraPreview
                        model: modelCameraList
                        Layout.fillWidth: true

                        onCurrentIndexChanged: {
                            cameraManager.setPreviewCameraIndex(idCbSelectCameraPreview.currentIndex)
                        }
                    }
                }
            }

            // -- SPACER
            Item {
                // Spacer
                Layout.fillWidth: true
            }

            Connections {
                target : cameraManager
                function onPreviewCameraIndexChanged(p_index) {
                    if( idCbSelectCameraPreview.currentIndex !== p_index ) {
                        idCbSelectCameraPreview.currentIndex = p_index;
                    }
                    if( -1 === p_index ) {
                        idCameraPreview.stop()
                    } else {
                        if( 1 === globalManager.guiPage ) {
                            if(!cameraManager.isPreviewCameraIp) {
                                idCameraPreview.deviceId = QtMultimedia.availableCameras[p_index].deviceId;
                                idCameraPreview.start()
                            }
                        }
                    }
                }
            }
        }




//        ListView {
//            id: idLinkList
//            width: 200
//            height: 250
//            Layout.fillHeight: true
//            Layout.fillWidth: true
//            Layout.leftMargin: 40


//            property var linkList : currentVector.getLinkManager().getLinkModel()
//            model: linkList
//            delegate: DeviceLinkConfig {

//                idCbLinkType.currentIndex: link_type-1  // enum 0 = unknow
//                idCbLinkType.visible: false // not available for now
//                idCbLinkComPort.model: serial_port_list
//                idCbLinkComPort.enabled: link_state === 0 /* disconnected */ || link_state === 3 /* disconnecting */ || link_state === 4 /* disconnected */
//                idBtLinkConnect.text : link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/? qsTr("Déconnecter") : qsTr("Connecter")
//                linkName : link_name
//                linkState: link_state

//                Connections {
//                    target : idBtLinkConnect
//                    function onPressed() {
//                        if( link_state === 1 /*Connecting*/|| link_state === 2 /*Connected*/ ) {
//                            currentVector.getLinkManager().disconnectLink(link_name);
//                        } else {
//                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
//                        }
//                    }

//                }

//                Connections {
//                    target : idCbLinkComPort
//                    function onActivated() {
//                        if( idCbLinkComPort.currentText !== currentVector.getLinkManager().getComPort(link_name) ) {
//                            currentVector.getLinkManager().setComPort(link_name, idCbLinkComPort.currentText);
//                        }
//                    }
//                }

//                Connections {
//                    target : idCbProduct
//                    function onCurrentIndexChanged() {
//                        if( currentVector.prodId !== idCbProduct.currentIndex+1 ) {
//                            currentVector.prodId = idCbProduct.currentIndex+1; // combo doesn't start to "undef"
//                        }
//                    }
//                }

//                Component.onCompleted: {
//                    idCbLinkComPort.currentIndex = idCbLinkComPort.find(serial_port);
//                    idCbProduct.currentIndex = currentVector.prodId-1
//                }

//            }
//        }

        // -- SPACER
        Item {
            // Spacer
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
