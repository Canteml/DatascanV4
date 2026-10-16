import QtQuick 2.4
import QtQuick.Controls 2.15

// Button that emits stepped() once on press, then repeatedly ONLY while it is held down.
// Replaces the built-in autoRepeat, which could keep firing after the button was released.
Button {
    id: idRepeatButton

    property int repeatDelay: 400      // ms before the first repetition
    property int repeatInterval: 100   // ms between repetitions

    signal stepped()

    function stopRepeat() {
        idRepeatTimer.stop()
    }

    onPressed: {
        stepped()
        idRepeatTimer.interval = repeatDelay
        idRepeatTimer.restart()
    }
    onReleased: stopRepeat()
    onCanceled: stopRepeat()
    onPressedChanged: if (!pressed) stopRepeat()
    onEnabledChanged: if (!enabled) stopRepeat()
    onVisibleChanged: if (!visible) stopRepeat()

    Timer {
        id: idRepeatTimer
        repeat: true
        onTriggered: {
            // Safety net: never repeat if the button is no longer actually held
            if (!idRepeatButton.pressed || !idRepeatButton.enabled || !idRepeatButton.visible) {
                stop()
                return
            }
            interval = idRepeatButton.repeatInterval
            idRepeatButton.stepped()
        }
    }
}
