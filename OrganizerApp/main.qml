import QtQuick
import QtQuick.Window

Window {
    width: 400
    height: 600
    visible: true
    title: "Органайзер приоритетов"

    ListView {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        model: appController.taskListModel

        delegate: Rectangle {
            width: ListView.view.width
            height: 60
            color: "#f0f0f0"
            radius: 8

            Column {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 12

                Text {
                    text: model.text
                    font.pixelSize: 16
                }
                Text {
                    text: "Приоритет: " + model.priority
                    font.pixelSize: 12
                    color: "#666"
                }
            }
        }
    }
}