import QtQuick 2.5
import QtQuick.Window 2.5
import QtQuick.Controls 2.5

Popup {
    // title: qsTr("GeoMessenger")
    closePolicy: Popup.CloseOnEscape
    modal: true
    focus: true

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
    }

    Rectangle {
        id: passwordTextFild

        width: 300
        height: 50
        anchors.centerIn: parent

        color: "white"
        border.color: mainWindow.logInError ? "red" : "black"
        radius: height/5

        property bool showPass : false

        TextField {
            id: passwordTextFildin

            width: parent.width - parent.height
            height: parent.height
            anchors.top: parent.top
            anchors.left: parent.left
            font.pointSize: 12

            placeholderText: "Password"

            echoMode:passwordTextFild.showPass ? TextInput.Normal : TextInput.Password

            background: Rectangle {
                radius: parent.height/5
                color: "transparent"
                border.color: "transparent"
            }

            text: qsTr("very-secure-password-for-knav-alice")
        }

        CircleButton {
            id: showPassword

            width: parent.height
            height: parent.height
            anchors.right: parent.right
            anchors.top: parent.top

            imageSource: passwordTextFild.showPass ? "/labels/eye.png" : "/labels/hide.png"

            onClicked: {
                passwordTextFild.showPass = !passwordTextFild.showPass
            }
        }
    }


    FancyTextField {
        id: emailTextFild

        width: passwordTextFild.width
        height: passwordTextFild.height
        anchors.bottom: passwordTextFild.top
        anchors.horizontalCenter: passwordTextFild.horizontalCenter
        anchors.margins: 30

        borderColor: mainWindow.logInError ? "red" : "black"

        placeholderText: "Email"

        text: qsTr("knav.alice@macaw.me")
    }


    Item {
        width: passwordTextFild.width
        height: passwordTextFild.height
        anchors.top: passwordTextFild.bottom
        anchors.horizontalCenter: passwordTextFild.horizontalCenter
        anchors.margins: 30

        FancyButton {
            id: okLogIn
            anchors.left: parent.left
            anchors.top: parent.top
            height: parent.height
            width: parent.width/2 - 10

            text: qsTr("OK")

            onClicked: {
                mainWindow.connectToServer(emailTextFild.text, passwordTextFildin.text);
                // mainWindow.logInFlag = true;
                // popupLogIn.close();
            }
        }

        FancyButton {
            id: cancelLogIn
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height
            width: parent.width/2 - 10

            text: qsTr("Cancel")

            onClicked: popupLogIn.close()
        }
    }
}
