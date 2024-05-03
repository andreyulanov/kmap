import QtQuick 2.5
import QtQuick.Controls 2.5

Button {

    height: 60

    property string imageSource: "/labels/user.png"

    property string name
    property string time
    property string lastMessage

    background: Rectangle {
        border.color: "transparent"
        color: parent.hovered ? "silver" : "white"
    }

    Item {
        id: cardImage
        height: parent.height
        width: parent.height
        anchors.left: parent.left
        anchors.top: parent.top


        Image {
            source: imageSource
            anchors.centerIn: parent
            height: parent.height * 0.7
            width: parent.width * 0.7
        }
    }

    Text {
        id: cardTime
        anchors.right: parent.right
        anchors.top: parent.top
        height: parent.height/2
        width: parent.height
        anchors.topMargin: 10

        color: "gray"

        text: time
    }

    Text {
        id: cardName
        height: parent.height/2
        anchors.left: cardImage.right
        anchors.right: cardTime.left
        anchors.top: parent.top
        anchors.topMargin: 10

        font.bold: true
        text: name
    }

    Text {
        id: cardLastMessage

        height: parent.height/2
        anchors.top: parent.verticalCenter
        anchors.bottom: parent.bottom
        anchors.left: cardImage.right
        anchors.right: parent.right

        color: "gray"

        text: lastMessage
    }
}
