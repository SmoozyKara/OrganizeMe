import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: window
    width: 400
    height: 600
    visible: true
    title: "Органайзер приоритетов"

    property int currentListId: -1
    property string currentListName: ""

    // Экран 1: список списков
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        visible: currentListId === -1

        // Форма создания нового списка
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
            height: parent.height - 50
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

                // Кнопка удаления списка - справа, поверх карточки.
                // Отдельная MouseArea внутри неё "перехватывает" клик,
                // чтобы не срабатывал переход в список (см. ниже).
                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Удалить"
                    onClicked: appController.organizerModel.deleteList(model.listId)
                }

                MouseArea {
                    anchors.fill: parent
                    // Оставляем место под кнопку справа свободным от
                    // перехвата клика, чтобы кнопка "Удалить" работала.
                    anchors.rightMargin: 80
                    onClicked: {
                        currentListId = model.listId
                        currentListName = model.name
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
        visible: currentListId !== -1

        Row {
            spacing: 12
            Button {
                text: "< Назад"
                onClicked: currentListId = -1
            }
            Text {
                text: currentListName
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Форма добавления задачи
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

            model: currentListId !== -1
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
}
