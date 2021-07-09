import QtQuick 2.4

BannerVectorInfoForm {

//    idLePosition.text : currentVector.funcPosition().paramLBscanPosMmX
    Connections {
        target : currentVector.funcPosition()
        function onParamLBscanPosMmXChanged(p_value) {
            idLePosition.text = p_value;
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
        idLePosition.text = currentVector.funcPosition().paramLBscanPosMmX;
    }

}
