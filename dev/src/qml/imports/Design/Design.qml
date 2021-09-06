
pragma Singleton
import QtQuick 2.15

QtObject {

    // Button Dark theme (Blue)
    readonly property color colButtonDarkBackground: "#003B5F"
    readonly property color colButtonDarkText: "#FFFFFF"
    readonly property color colButtonDarkSelectedBackground: "#C9C7C7"//"#9C9B9B"
    readonly property color colButtonDarkSelectedText: "#000000" // "#FFFFFF"
    readonly property color colButtonDarkHoveredBackground: "#C9C7C7"
    readonly property color colButtonDarkHoveredText: "#FF8200"

    // Button light theme
    readonly property color colButtonBackground: "#FFFFFF"
    readonly property color colButtonText: "#000000"
//    readonly property color colButtonBackground: "#"
//    readonly property color colButtonText: "#"
    readonly property color colButtonSelectedBackground: "#C9C7C7"
    readonly property color colButtonSelectedText: "#000000"
    readonly property color colButtonHoveredBackground: "#E5E5E5"
    readonly property color colButtonHoveredText: "#000000"
    readonly property color colButtonBorder: "black"

    readonly property color colFrameBorder: "black"
    readonly property color colSeparator: "#9C9B9B"

    // Form Section

    readonly property color colFromSection: "#FF8200"   // orange
}

