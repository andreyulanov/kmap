import QtQuick 2.5
import QtQuick.Controls 2.5

TextField {

    property color borderColor
    font.pointSize: 12

    background: Rectangle {
        radius: parent.height/5
        border.color: borderColor
    }
}
