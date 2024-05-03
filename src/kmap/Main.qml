import QtQuick 2.5
import QtQuick.Window 2.5
import QtQuick.Controls 2.5


Item {
    id: mainWindow
    visible: true
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "white"
        border.color: "white"
    }

    signal connectToServer(login: string, password: string)
    property bool logInFlag: false
    property bool logInError: false

    LogIn {
        id: popupLogIn
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.8
    }

    Drawer {
        id: drawerMenu
        height: parent.height
        width: 400

        FancyButton{
            id: logOutButton
            anchors.centerIn: parent
            width: parent.width - 10

            text: qsTr("Log Out")

            onClicked: {
                kClient.disconnectFromServer();
            }
        }
    }

    Item {
        id: chatArea
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: mapAria.left

        Item{
            visible: logInFlag
            id: chatAreaHead

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            height: 50

            CircleButton {
                id: menuButton
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.height
                height: parent.height

                imageSource: "/labels/menu.png"

                onClicked: drawerMenu.open()
            }

            FancyTextField {
                id: chatSearch

                anchors.right: parent.right
                anchors.left: menuButton.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 5

                placeholderText: "Search"
            }
        }

        Item{
            id: logInArea

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            height: 50

            visible: !logInFlag

            FancyButton {
                id: logButtont

                anchors.fill: parent
                anchors.margins: 5

                text: qsTr("Log In")
                onClicked: {
                    popupLogIn.open();
                    logInError = false;
                }
            }
        }

        Item{
            id: chats
            visible: logInFlag

            anchors.top: chatAreaHead.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            height: 50

            ContactCard {
                width: parent.width

                name: "User Name"
                time: "12:24"
                lastMessage: "Last Message"
            }
        }
    }

    Item {
        id: mapAria

        anchors.top: parent.top
        anchors.bottom: parent.bottom
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

            Rectangle {
                color: "green"
                anchors.fill: parent
            }

            FancyButton {
                anchors.centerIn: parent

                text: qsTr("Open map [WIP]")
            }
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

    Connections {
        target: kClient
        onConnected: {
            onConnected();
        }
        onError:{
            onError();
        }
        onDisconnected: {
            onDisconnected();
        }
    }

    function onConnected() {
        logInFlag = true;
        popupLogIn.close();
    }

    function onError() {
        logInError = "true";
    }

    function onDisconnected(){
        logInFlag = false;
        drawerMenu.close();
    }
}


