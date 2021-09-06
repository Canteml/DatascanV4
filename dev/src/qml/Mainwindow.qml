import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

MainwindowForm {
    id: mainwindowForm

    readonly property int page0_project : 0;
    readonly property int page1_prepare : 1;
    readonly property int page2_supervise : 2;
    readonly property int page3_scan: 3;
    readonly property int page4_help: 4;

    property var displayMenu: false

    btMainMenu.onHoveredChanged: checkMainMenuHovered();
    btCentralPage0.onHoveredChanged: checkMainMenuHovered();
    btCentralPage1.onHoveredChanged: checkMainMenuHovered();
    btCentralPage2.onHoveredChanged: checkMainMenuHovered();
    btCentralPage3.onHoveredChanged: checkMainMenuHovered();
    btCentralPage4.onHoveredChanged: checkMainMenuHovered();


    btCentralPage0.onPressed: {globalManager.guiPage = page0_project; displayMenu = false;}
    btCentralPage1.onPressed: {globalManager.guiPage = page1_prepare; displayMenu = false;}
    btCentralPage2.onPressed: {globalManager.guiPage = page2_supervise; displayMenu = false;}
    btCentralPage3.onPressed: {globalManager.guiPage = page3_scan; displayMenu = false;}
    btCentralPage4.onPressed: {globalManager.guiPage = page4_help; displayMenu = false;}

    btBannerTitle.onPressed: { globalManager.guiPage = globalManager.guiPage === 3 ? 2 : 3;}

    onDisplayMenuChanged: {
        if( displayMenu ) {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuClicked.png"
            btMainMenu.background.color = Design.colButtonDarkBackground

        } else {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuNormal.png"
            btMainMenu.background.color = Design.colButtonDarkHoveredBackground
        }

        btCentralPage0.visible = displayMenu;
        btCentralPage1.visible = displayMenu;
        btCentralPage2.visible = displayMenu;
        btCentralPage3.visible = displayMenu;
        btCentralPage4.visible = displayMenu;
    }

    Connections {
        target : globalManager
        function onGuiPageChanged() {
            updateCentralPage();
        }
    }

    function updateCentralPage() {
        btCentralPage0.background.color = globalManager.guiPage === page0_project ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage1.background.color = globalManager.guiPage === page1_prepare ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage2.background.color = globalManager.guiPage === page2_supervise ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage3.background.color = globalManager.guiPage === page3_scan ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage4.background.color = globalManager.guiPage === page4_help ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        slCentral.currentIndex = globalManager.guiPage;
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




