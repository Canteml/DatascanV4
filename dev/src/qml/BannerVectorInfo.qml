import QtQuick 2.4

BannerVectorInfoForm {

//    idLePosition.text : currentVector.funcPosition().paramLBscanPosMmX
    Connections {
        target : currentVector.funcPosition()
        function onParamLBscanPosMmXChanged() {
            updateLePosition();
        }
    }

    Connections {
        target : idBtPositionPlus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(10);
        }
    }

    Connections {
        target : idBtPositionMinus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(-10);
        }
    }

    Connections {
        target : currentVector.getDeviceManager()
        function onStateChanged() {
            if( 3 === currentVector.getDeviceManager().state ) {
                btVectorConnection.icon.source = "qrc:///img/icons/iconConnected.png"
            } else {
                btVectorConnection.icon.source = "qrc:///img/icons/iconDisconnected.png"
            }
        }
    }

    Component.onCompleted: {
        updateLePosition();
    }

    function updateLePosition() {
        idLePosition.text = Math.round(currentVector.funcPosition().paramLBscanPosMmX/10)/100;
    }

}
