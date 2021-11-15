import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {
    id: idRectTopCam

    //color: "#0055AA"


//    Camera {
//        id: idCamera


////      imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash

////      exposure {
////          exposureCompensation: -1.0
////          exposureMode: Camera.ExposurePortrait
////      }


//        imageCapture {
//            onImageCaptured: {
//                photoPreview.source = preview  // Show the preview in an Image
//            }
//        }
//    }

    Connections {
        target : currentVector;
        function onHasCameraChanged() {
            updateCamera();
        }
    }

//    function updateCamera() {
//        if( currentVector.hasCamera ) {
//            idCamera.start();
//        } else {
//            idCamera.stop();
//        }
//    }

    Component.onCompleted: {
        updateCamera();
    }

//    ColumnLayout {
//        anchors.fill: parent
//        spacing: 0


        Rectangle {
            border.color: "black"
            color: "transparent"
            anchors.fill: parent
        }

//        VideoOutput {
//            source: idCamera
////            Layout.fillHeight: true;
////            Layout.fillWidth: true;
//            anchors.margins: 1
//            anchors.fill: parent
//            focus : visible // to receive focus and capture key events when visible
//        }

        ComboBox {
            id: idCbCam

            anchors.margins: 1
            //Layout.fillWidth: true;
            anchors.right: parent.right
            width: 250
            background: Rectangle {
                    color:"#FFFFFF"
                    opacity: 0.5
            }

            model: QtMultimedia.availableCameras
            delegate : ItemDelegate {
               text: index + " : " + modelData.displayName

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        idCamera.deviceId = modelData.deviceId
                        idCbCam.displayText = text
                        console.log("Cam ID : " + idCamera.deviceId)
                    }
                }
            }

//           onCurrentIndexChanged: {
//               idCamera.deviceId = modelData.deviceId
//           }


//           delegate: Text {
//               text: index + " " + modelData.displayName + modelData.deviceId

//               MouseArea {
//                   anchors.fill: parent
//                   onClicked: camera.deviceId = modelData.deviceId
//               }
//           }


       }

//    }
}
