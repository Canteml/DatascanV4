
pragma Singleton
import QtQuick 2.15

QtObject {

    readonly property FontLoader loaderRobotoCondensedRegular: FontLoader {
        source: "qrc:///fonts/RobotoCondensed-Regular.ttf"
    }

    readonly property FontLoader loaderRobotoCondensedBold: FontLoader {
        source: "qrc:///fonts/RobotoCondensed-Bold.ttf"
    }

    // Usage: font.family: Fonts.robotoCondensed
    readonly property string robotoCondensed: loaderRobotoCondensedRegular.name
}
