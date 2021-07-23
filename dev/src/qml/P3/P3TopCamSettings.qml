import QtQuick 2.4

P3TopCamSettingsForm {
    idBtCam.onPressed: {
        idSlP4TopCamSettings.currentIndex === 0 && idSlP4TopCamSettings.visible ? idSlP4TopCamSettings.visible = false : idSlP4TopCamSettings.visible = true;
        idSlP4TopCamSettings.currentIndex = 0;
    }

    idBtAScanSettings.onPressed: {
        idSlP4TopCamSettings.currentIndex === 1 && idSlP4TopCamSettings.visible ? idSlP4TopCamSettings.visible = false : idSlP4TopCamSettings.visible = true;
        idSlP4TopCamSettings.currentIndex = 1;
    }

    idBtBScanSettings.onPressed: {
        idSlP4TopCamSettings.currentIndex === 2 && idSlP4TopCamSettings.visible ? idSlP4TopCamSettings.visible = false : idSlP4TopCamSettings.visible = true;
        idSlP4TopCamSettings.currentIndex = 2;
    }


    Connections {
        target : currentVector
        function onProdIdChanged() {
            configureProdUId();
        }
    }

    Component.onCompleted: {
        configureProdUId();
    }

    function configureProdUId() {
        // has camera ?
        if( currentVector.prodId === 3 ||  currentVector.prodId === 4 ||  currentVector.prodId === 5 ) {
            // yes
            idBtCam.visible = true;
            idSlP4TopCamSettings.currentIndex = 0;
            idSlP4TopCamSettings.visible = true;
        } else {
            idBtCam.visible = false;
            idSlP4TopCamSettings.currentIndex = 0;
            idSlP4TopCamSettings.visible = false;
        }


    }

}
