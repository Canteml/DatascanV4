import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

MainwindowForm {
    id: mainwindowForm

    property var displayMenu: false
    property var centralPageSelected: 3

    btMainMenu.onHoveredChanged: checkMainMenuHovered();
    btCentralPage0.onHoveredChanged: checkMainMenuHovered();
    btCentralPage1.onHoveredChanged: checkMainMenuHovered();
    btCentralPage2.onHoveredChanged: checkMainMenuHovered();
    btCentralPage3.onHoveredChanged: checkMainMenuHovered();
    btCentralPage4.onHoveredChanged: checkMainMenuHovered();


    btCentralPage0.onClicked: {centralPageSelected = 0; displayMenu = false;}
    btCentralPage1.onClicked: {centralPageSelected = 1; displayMenu = false;}
    btCentralPage2.onClicked: {centralPageSelected = 2; displayMenu = false;}
    btCentralPage3.onClicked: {centralPageSelected = 3; displayMenu = false;}
    btCentralPage4.onClicked: {centralPageSelected = 4; displayMenu = false;}

    onDisplayMenuChanged: {
        if( displayMenu ) {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuClicked.png"
            btMainMenu.background.color = "#003B5F"

        } else {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuNormal.png"
            btMainMenu.background.color = "#C9C7C7"
        }

        btCentralPage0.visible = displayMenu;
        btCentralPage1.visible = displayMenu;
        btCentralPage2.visible = displayMenu;
        btCentralPage3.visible = displayMenu;
        btCentralPage4.visible = displayMenu;
    }

    onCentralPageSelectedChanged: {
        updateCentralPage();
    }

    function updateCentralPage() {
        btCentralPage0.background.color = centralPageSelected === 0 ? "#9C9B9B" : "#003B5F"
        btCentralPage1.background.color = centralPageSelected === 1 ? "#9C9B9B" : "#003B5F"
        btCentralPage2.background.color = centralPageSelected === 2 ? "#9C9B9B" : "#003B5F"
        btCentralPage3.background.color = centralPageSelected === 3 ? "#9C9B9B" : "#003B5F"
        btCentralPage4.background.color = centralPageSelected === 4 ? "#9C9B9B" : "#003B5F"

        slCentral.currentIndex = centralPageSelected;
    }

    function checkMainMenuHovered() {
        var h = btMainMenu.hovered;
        h +=  btCentralPage0.hovered;
        h +=  btCentralPage1.hovered;
        h +=  btCentralPage2.hovered;
        h +=  btCentralPage3.hovered;
        h +=  btCentralPage4.hovered;

        displayMenu = h;
    }

    Component.onCompleted: {
        checkMainMenuHovered();
        updateCentralPage();
    }
}



/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
