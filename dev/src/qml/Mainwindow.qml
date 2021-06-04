import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

MainwindowForm {
    id: mainwindowForm

    readonly property int page0_project : 0;
    readonly property int page1_prepare : 1;
    readonly property int page2_supervise : 2;
    readonly property int page3_scan: 3;
    readonly property int page4_help: 4;

    property var displayMenu: false
    property var centralPageSelected: page3_scan

    btMainMenu.onHoveredChanged: checkMainMenuHovered();
    btCentralPage0.onHoveredChanged: checkMainMenuHovered();
    btCentralPage1.onHoveredChanged: checkMainMenuHovered();
    btCentralPage2.onHoveredChanged: checkMainMenuHovered();
    btCentralPage3.onHoveredChanged: checkMainMenuHovered();
    btCentralPage4.onHoveredChanged: checkMainMenuHovered();


    btCentralPage0.onPressed: {centralPageSelected = page0_project; displayMenu = false;}
    btCentralPage1.onPressed: {centralPageSelected = page1_prepare; displayMenu = false;}
    btCentralPage2.onPressed: {centralPageSelected = page2_supervise; displayMenu = false;}
    btCentralPage3.onPressed: {centralPageSelected = page3_scan; displayMenu = false;}
    btCentralPage4.onPressed: {centralPageSelected = page4_help; displayMenu = false;}

    btBannerTitle.onPressed: { centralPageSelected = centralPageSelected === 3 ? 2 : 3;}

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
        btCentralPage0.background.color = centralPageSelected === page0_project ? "#9C9B9B" : "#003B5F"
        btCentralPage1.background.color = centralPageSelected === page1_prepare ? "#9C9B9B" : "#003B5F"
        btCentralPage2.background.color = centralPageSelected === page2_supervise ? "#9C9B9B" : "#003B5F"
        btCentralPage3.background.color = centralPageSelected === page3_scan ? "#9C9B9B" : "#003B5F"
        btCentralPage4.background.color = centralPageSelected === page4_help ? "#9C9B9B" : "#003B5F"

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




