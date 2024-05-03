import QtQuick 2.5
import QtQuick.Controls 2.5

Button {

    property string imageSource: ""

    background: Rectangle {
        radius: width/2
        border.color: "transparent"
        color: "transparent"
    }

    Rectangle {
        visible: parent.hovered
        anchors.centerIn: parent
        width: parent.height * 0.8
        height: parent.height * 0.8

        radius: height/2

        color: "silver"
    }

    Image {
        source: parent.imageSource
        anchors.centerIn: parent
        height: parent.height/2
        width: parent.width/2
    }   
}
