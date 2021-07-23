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
            console.log("Pressed");
        }
    }

    Connections {
        target : idBtPositionMinus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(-10);
            console.log("Pressed");
        }
    }

    Component.onCompleted: {
        updateLePosition();
    }

    function updateLePosition() {
        idLePosition.text = Math.round(currentVector.funcPosition().paramLBscanPosMmX/10)/100;
    }

}
