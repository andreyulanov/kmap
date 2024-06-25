import QtQuick 2.5

Item {

    x: 300
    width: parent.width - x

    Item{
        id: mapAreaHead

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        height: 50

        Rectangle {
            anchors.fill: parent
            color: "white"
            border.color: "gainsboro"
        }
    }

    Item{
        id: map

        anchors.top: mapAreaHead.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
    }

    MouseArea {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: mapAria.left
        width: 5
        cursorShape: Qt.SizeHorCursor

        drag.target: mapAria
        drag.axis: Drag.XAxis
        drag.minimumX: 200
        drag.maximumX: 1500
    }
}
