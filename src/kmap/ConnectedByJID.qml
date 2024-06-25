import QtQuick 2.5
import QtQuick.Window 2.5
import QtQuick.Controls 2.5

Popup {

    closePolicy: Popup.CloseOnEscape
    modal: true
    focus: true

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 200 }
    }

    FancyTextField {
        id: JIDTextFild

        width: 300
        height: 50
        anchors.centerIn: parent

        borderColor: root.logInError ? "red" : "black"

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
                root.connectToServer(emailTextFild.text, passwordTextFildin.text);
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
