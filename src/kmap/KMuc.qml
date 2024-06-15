import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: root
    Rectangle {
        width: 360
        height: 360

        ListView {
            id: view

            anchors.margins: 10
            anchors.fill: parent
            spacing: 10
            model: _mucRoomsModel
            clip: true

            highlight: Rectangle {
                color: "skyblue"
            }
            highlightFollowsCurrentItem: true

            header: Rectangle {
                width: view.width
                height: 40
                Grid {
                    id: addFormGrid
                    TextField {
                        id: roomJidTextField
                        width: 250
                        text: _mucBackEnd.room_jid
                        placeholderText: qsTr("Room Jid")
                        onEditingFinished: _mucBackEnd.room_jid = text
                        onFocusChanged: color = "black"
                    }
                    Button {
                        text: "Add"
                        onClicked: {
                            _mucBackEnd.add();
                            console.log("_mucBackend.roomJid: " + _mucBackEnd.room_jid)
                        }
                    }
                }
                Connections {
                    target: _mucBackEnd
                    function onInvalidJid() {
                        roomJidTextField.color = "red"
                    }
                }
            }

            delegate: Item {
                id: listDelegate

                property var view: ListView.view
                property var isCurrent: ListView.isCurrentItem

                width: view.width
                height: 40

                Rectangle {
                    anchors.margins: 5
                    anchors.fill: parent
                    radius: height / 2
                    border {
                        color: "black"
                        width: 1
                    }

                    Text {
                        anchors.centerIn: parent
                        renderType: Text.NativeRendering
                        text: "%1 (%2)\n%3".arg(model.name).arg(model.jid).arg(model.subject)
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: view.currentIndex = model.index
                    }
                }
            }

            footer: Rectangle {
                width: view.width
                height: 40
                color: "red"
                Text {
                    id: fixMeNote
                    anchors.centerIn: parent
                    text: qsTr("Fix my design please...")
                }
            }
        }
    }
}
