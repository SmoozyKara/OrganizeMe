import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: window
    width: 400
    height: 600
    visible: true
    title: "Органайзер приоритетов"

    // "lists" - список списков, "tasks" - задачи одного списка,
    // "summary" - сводный список всех задач сразу.
    property string currentScreen: "lists"
    property int currentListId: -1
    property string currentListName: ""

    // Экран 1: список списков
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        visible: currentScreen === "lists"

        Row {
            spacing: 8
            Button {
                text: "Все задачи"
                onClicked: {
                    appController.summaryModel.refresh()
                    currentScreen = "summary"
                }
            }
        }

        Row {
            spacing: 8
            TextField {
                id: newListName
                placeholderText: "Название списка"
                width: 160
            }
            TextField {
                id: newListColor
                placeholderText: "#RRGGBB"
                width: 100
            }
            Button {
                text: "Добавить"
                onClicked: {
                    if (newListName.text.length === 0) return
                    appController.organizerModel.createList(newListName.text, newListColor.text)
                    newListName.text = ""
                    newListColor.text = ""
                }
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 100
            spacing: 8

            model: appController.organizerModel

            delegate: Rectangle {
                width: ListView.view.width
                height: 60
                color: model.color
                radius: 8

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.name
                    color: "white"
                    font.pixelSize: 18
                }

                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Удалить"
                    onClicked: appController.organizerModel.deleteList(model.listId)
                }

                MouseArea {
                    anchors.fill: parent
                    anchors.rightMargin: 80
                    onClicked: {
                        currentListId = model.listId
                        currentListName = model.name
                        currentScreen = "tasks"
                    }
                }
            }
        }
    }

    // Экран 2: задачи внутри выбранного списка
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        visible: currentScreen === "tasks"

        Row {
            spacing: 12
            Button {
                text: "< Назад"
                onClicked: currentScreen = "lists"
            }
            Text {
                text: currentListName
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row {
            spacing: 8
            TextField {
                id: newItemText
                placeholderText: "Текст задачи"
                width: 160
            }
            SpinBox {
                id: newItemPriority
                from: 1
                to: 10
                value: 1
            }
            Button {
                text: "Добавить"
                onClicked: {
                    if (newItemText.text.length === 0) return
                    appController.getTaskListModel(currentListId).addItem(
                        newItemPriority.value, newItemText.text)
                    newItemText.text = ""
                }
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 100
            spacing: 8

            model: currentScreen === "tasks"
                   ? appController.getTaskListModel(currentListId)
                   : null

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

                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Удалить"
                    onClicked: appController.getTaskListModel(currentListId).removeItem(model.itemId)
                }
            }
        }
    }

    // Экран 3: сводный список всех задач
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        visible: currentScreen === "summary"

        Row {
            spacing: 12
            Button {
                text: "< Назад"
                onClicked: currentScreen = "lists"
            }
            Text {
                text: "Все задачи"
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 60
            spacing: 8

            // Модель уже "смотрит" в кэш, посчитанный при нажатии
            // кнопки "Все задачи" - здесь только отображение.
            model: appController.summaryModel

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
}
