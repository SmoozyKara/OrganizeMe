import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic
import QtQuick.Layouts

Window {
    id: window
    width: 400
    height: 600
    visible: true
    title: "Органайзер приоритетов"

    property string currentScreen: "lists"
    property int currentListId: -1
    property string currentListName: ""

    function contrastColor(bgColor) {
        var c = Qt.color(bgColor)
        var luminance = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b
        return luminance > 0.5 ? "black" : "white"
    }

    // Компактная кнопка-"чип": прямоугольник со скруглением вместо
    // стандартного QtQuick.Controls Button - полный контроль над видом.
    component Chip: Rectangle {
        id: chip
        property string label: ""
        property color chipColor: "#e0e0e0"
        property color labelColor: "black"
        signal clicked()

        width: chipText.implicitWidth + 20
        height: 32
        radius: 16
        color: chip.chipColor

        Text {
            id: chipText
            anchors.centerIn: parent
            text: chip.label
            color: chip.labelColor
            font.pixelSize: 13
        }

        MouseArea {
            anchors.fill: parent
            onClicked: chip.clicked()
        }
    }

    // Минималистичная иконка-крестик - две тонкие линии крест-накрест,
    // без заливки и обводки, вместо символа шрифта в кружке. Символы
    // шрифта визуально "тяжелее" и меньше похожи на настоящую иконку.
    component DeleteIcon: Item {
        id: delIcon
        property color iconColor: "black"
        property real lineOpacity: 0.55
        signal clicked()

        width: 30
        height: 30

        Rectangle {
            anchors.centerIn: parent
            width: 14
            height: 1.5
            radius: 1
            color: delIcon.iconColor
            opacity: delIcon.lineOpacity
            rotation: 45
        }
        Rectangle {
            anchors.centerIn: parent
            width: 14
            height: 1.5
            radius: 1
            color: delIcon.iconColor
            opacity: delIcon.lineOpacity
            rotation: -45
        }

        MouseArea {
            anchors.fill: parent
            onClicked: delIcon.clicked()
        }
    }
    // Своё поле ввода с фиксированным светлым видом - не зависит от
    // системной тёмной/светлой темы (стандартный TextField иначе
    // наследует системную тему и может стать чёрным на тёмной теме).
    component StyledTextField: TextField {
        id: styledField
        color: "black"
        placeholderTextColor: "#999"
        leftPadding: 12
        rightPadding: 12

        background: Rectangle {
            radius: 10
            color: "white"
            border.color: styledField.activeFocus ? "#37474F" : "#dddddd"
            border.width: styledField.activeFocus ? 2 : 1
        }
    }

    // Компактный степпер приоритета - минус/значение/плюс в одном
    // ряду, вместо казённого стандартного SpinBox.
    component CompactStepper: Row {
        id: stepper
        property int value: 1
        property int from: 1
        property int to: 10
        signal valueEdited(int newValue)

        spacing: 2
        height: 28

        Rectangle {
            id: minusBtn
            width: 28
            height: 28
            radius: 14
            color: "#00000010"
            opacity: stepper.value > stepper.from ? 1.0 : 0.35

            // Минус - одна тонкая горизонтальная линия, а не символ
            // шрифта (у разных шрифтов "–" рисуется по-разному и
            // выглядит нечётко на таком маленьком размере).
            Rectangle {
                anchors.centerIn: parent
                width: 12
                height: 2
                radius: 1
                color: "#444"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (stepper.value > stepper.from) {
                        stepper.value--
                        stepper.valueEdited(stepper.value)
                    }
                }
            }
        }

        Text {
            width: 26
            height: 28
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: stepper.value
            font.pixelSize: 14
            font.bold: true
        }

        Rectangle {
            id: plusBtn
            width: 28
            height: 28
            radius: 14
            color: "#00000010"
            opacity: stepper.value < stepper.to ? 1.0 : 0.35

            // Плюс - те же две тонкие линии крест-накрест, что и
            // крестик удаления, только без поворота на 45° - так
            // визуально сразу видно, что это одно семейство иконок.
            Rectangle {
                anchors.centerIn: parent
                width: 12
                height: 2
                radius: 1
                color: "#444"
            }
            Rectangle {
                anchors.centerIn: parent
                width: 2
                height: 12
                radius: 1
                color: "#444"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (stepper.value < stepper.to) {
                        stepper.value++
                        stepper.valueEdited(stepper.value)
                    }
                }
            }
        }
    }

    readonly property var colorSwatches: {
        var result = []
        var hueSteps = 10
        var lightnessSteps = [0.45, 0.55, 0.65, 0.72, 0.8]
        for (var l = 0; l < lightnessSteps.length; l++) {
            for (var h = 0; h < hueSteps; h++) {
                result.push(Qt.hsla(h / hueSteps, 0.55, lightnessSteps[l], 1.0))
            }
        }
        return result
    }

    property bool colorPickerVisible: false
    property int colorPickerTargetListId: -1

    function openColorPicker(anchorItem, listId) {
        colorPickerTargetListId = listId
        colorPickerVisible = true
    }

    // Затемнение фона + перехват клика "мимо" для закрытия - лежит
    // прямо в дереве окна (не Popup из Controls), поэтому полностью
    // под нашим контролем: ни авто-позиционирования, ни авто-стилей.
    Rectangle {
        anchors.fill: parent
        color: "#00000030"
        visible: window.colorPickerVisible
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: window.colorPickerVisible = false
        }
    }

    Rectangle {
        id: colorPickerPanel
        visible: window.colorPickerVisible
        z: 101
        // Центрируем в окне вместо привязки к позиции клика - тогда
        // палитра гарантированно помещается на экран при любом
        // размере окна, независимо от того, где был клик.
        anchors.centerIn: parent
        width: Math.min(240, window.width - 40)
        height: Math.min(280, window.height - 80)
        radius: 14
        color: "white"
        border.color: "#00000020"
        border.width: 1

        // Клик внутри панели не должен долетать до затемнения сзади
        // и закрывать её - отдельная MouseArea "съедает" клик здесь.
        MouseArea {
            anchors.fill: parent
        }

        Flickable {
            anchors.fill: parent
            anchors.margins: 10
            contentWidth: width
            contentHeight: swatchGrid.implicitHeight
            clip: true

            GridLayout {
                id: swatchGrid
                width: parent.width
                columns: 6
                rowSpacing: 8
                columnSpacing: 8

                Repeater {
                    model: window.colorSwatches
                    delegate: Rectangle {
                        Layout.preferredWidth: 26
                        Layout.preferredHeight: 26
                        radius: 13
                        color: modelData
                        border.color: "#00000020"
                        border.width: 1

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                appController.organizerModel.changeColor(
                                    window.colorPickerTargetListId, modelData.toString())
                                window.colorPickerVisible = false
                            }
                        }
                    }
                }
            }
        }
    }

    // Всплывающий выбор приоритета (1-10) - тот же паттерн, что и
    // палитра цветов выше: свой Rectangle+Repeater, без Popup из
    // Controls, полностью под нашим контролем.
    property bool priorityPickerVisible: false
    property int priorityPickerTargetListId: -1
    property int priorityPickerTargetItemId: -1
    property int priorityPickerCurrentValue: 1

    function openPriorityPicker(listId, itemId, currentValue) {
        priorityPickerTargetListId = listId
        priorityPickerTargetItemId = itemId
        priorityPickerCurrentValue = currentValue
        priorityPickerVisible = true
    }

    Rectangle {
        anchors.fill: parent
        color: "#00000030"
        visible: window.priorityPickerVisible
        z: 100

        MouseArea {
            anchors.fill: parent
            onClicked: window.priorityPickerVisible = false
        }
    }

    Rectangle {
        id: priorityPickerPanel
        visible: window.priorityPickerVisible
        z: 101
        anchors.centerIn: parent
        // Размер считается от содержимого (pickerColumn), а не задан
        // магическим числом - раньше фиксированная высота 180
        // оставляла много пустого места под цифрами.
        width: pickerColumn.implicitWidth + 28
        height: pickerColumn.implicitHeight + 28
        radius: 16
        color: "white"
        border.color: "#37474F"
        border.width: 2

        MouseArea {
            anchors.fill: parent
        }

        Column {
            id: pickerColumn
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Выбери приоритет"
                font.pixelSize: 13
                color: "#666"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            GridLayout {
                columns: 5
                rowSpacing: 6
                columnSpacing: 6

                Repeater {
                    model: 15
                    delegate: Rectangle {
                        readonly property int priorityValue: index + 1
                        readonly property bool isCurrent: priorityValue === window.priorityPickerCurrentValue

                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        radius: 14
                        color: isCurrent ? "#37474F" : "#00000010"

                        Text {
                            anchors.centerIn: parent
                            text: parent.priorityValue
                            color: parent.isCurrent ? "white" : "black"
                            font.pixelSize: 12
                            font.bold: parent.isCurrent
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                appController.getTaskListModel(window.priorityPickerTargetListId)
                                    .changePriority(window.priorityPickerTargetItemId, parent.priorityValue)
                                window.priorityPickerVisible = false
                            }
                        }
                    }
                }
            }
        }
    }

    // Экран 1: список списков
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10
        visible: currentScreen === "lists"

        Chip {
            label: "Все задачи"
            chipColor: "#37474F"
            labelColor: "white"
            onClicked: {
                appController.summaryModel.refresh()
                currentScreen = "summary"
            }
        }

        RowLayout {
            width: parent.width
            spacing: 8
            StyledTextField {
                id: newListName
                Layout.fillWidth: true
                placeholderText: "Название списка"
            }
            Chip {
                label: "Добавить"
                chipColor: "#37474F"
                labelColor: "white"
                onClicked: {
                    if (newListName.text.length === 0) return
                    appController.organizerModel.createList(newListName.text)
                    newListName.text = ""
                }
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 100
            spacing: 8
            clip: true

            model: appController.organizerModel

            delegate: Rectangle {
                id: listCard
                width: ListView.view.width
                height: 64
                color: model.color
                radius: 14

                property color textColor: window.contrastColor(model.color)

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 12
                    spacing: 8

                    Text {
                        text: model.name
                        color: listCard.textColor
                        font.pixelSize: 18
                        font.bold: true
                        Layout.fillWidth: true
                        elide: Text.ElideRight

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                currentListId = model.listId
                                currentListName = model.name
                                currentScreen = "tasks"
                            }
                        }
                    }

                    // Кружок показывает сам текущий цвет списка -
                    // и служит переключателем цвета по тапу.
                    Rectangle {
                        id: colorSwatch
                        Layout.preferredWidth: 26
                        Layout.preferredHeight: 26
                        radius: 13
                        color: model.color
                        border.color: "#ffffff90"
                        border.width: 2

                        MouseArea {
                            anchors.fill: parent
                            onClicked: window.openColorPicker(colorSwatch, model.listId)
                        }
                    }

                    DeleteIcon {
                        iconColor: listCard.textColor
                        onClicked: appController.organizerModel.deleteList(model.listId)
                    }
                }
            }
        }
    }

    // Экран 2: задачи внутри выбранного списка
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10
        visible: currentScreen === "tasks"

        RowLayout {
            width: parent.width
            spacing: 12
            Chip {
                label: "< Назад"
                chipColor: "#37474F"
                labelColor: "white"
                onClicked: currentScreen = "lists"
            }
            Text {
                text: currentListName
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
            }
        }

        RowLayout {
            width: parent.width
            spacing: 8
            StyledTextField {
                id: newItemText
                Layout.fillWidth: true
                placeholderText: "Текст задачи"
            }
            Column {
                spacing: 2
                Text {
                    text: "Приоритет"
                    font.pixelSize: 10
                    color: "#888"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                CompactStepper {
                    id: newItemPriority
                    from: 1
                    to: 15
                    value: 1
                }
            }
            Chip {
                label: "Добавить"
                chipColor: "#37474F"
                labelColor: "white"
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
            height: parent.height - 110
            spacing: 8
            clip: true

            model: currentScreen === "tasks"
                   ? appController.getTaskListModel(currentListId)
                   : null

            delegate: Rectangle {
                id: taskCard
                // Защита: если у списка почему-то пустой/невалидный
                // цвет (например, остался от старых сохранённых
                // данных, созданных до автогенерации цвета) -
                // используем серый по умолчанию вместо падения в белый.
                property color baseColor: model.listColor && model.listColor.length > 0
                                           ? model.listColor : "#9e9e9e"
                property color itemColor: Qt.lighter(baseColor, Math.min(1.55, 1.0 + (model.priority - 1) * 0.035))
                property color textColor: window.contrastColor(itemColor)

                width: ListView.view.width
                height: taskContent.implicitHeight + 20
                color: itemColor
                radius: 12

                RowLayout {
                    id: taskContent
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 10
                    anchors.topMargin: 10
                    anchors.bottomMargin: 10
                    spacing: 8

                    Text {
                        Layout.fillWidth: true
                        text: model.text
                        color: taskCard.textColor
                        font.pixelSize: 16
                        wrapMode: Text.Wrap
                        verticalAlignment: Text.AlignVCenter
                    }

                    Column {
                        spacing: 6
                        Layout.alignment: Qt.AlignTop

                        DeleteIcon {
                            anchors.right: parent.right
                            iconColor: taskCard.textColor
                            onClicked: appController.getTaskListModel(currentListId).removeItem(model.itemId)
                        }

                        // Бейдж с числом приоритета - тап открывает
                        // всплывающий выбор значения (1-10). Не +/-,
                        // чтобы не путать со значком удаления рядом.
                        Rectangle {
                            anchors.right: parent.right
                            width: 30
                            height: 30
                            radius: 15
                            color: "#00000018"
                            border.color: taskCard.textColor
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: model.priority
                                color: taskCard.textColor
                                font.pixelSize: 14
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: window.openPriorityPicker(currentListId, model.itemId, model.priority)
                            }
                        }
                    }
                }
            }
        }
    }

    // Экран 3: сводный список всех задач
    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10
        visible: currentScreen === "summary"

        RowLayout {
            width: parent.width
            spacing: 12
            Chip {
                label: "< Назад"
                chipColor: "#37474F"
                labelColor: "white"
                onClicked: currentScreen = "lists"
            }
            Text {
                text: "Все задачи"
                font.pixelSize: 20
                font.bold: true
                Layout.fillWidth: true
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 60
            spacing: 8
            clip: true

            model: appController.summaryModel

            delegate: Rectangle {
                id: summaryCard
                property color baseColor: model.listColor && model.listColor.length > 0
                                           ? model.listColor : "#9e9e9e"
                property color itemColor: Qt.lighter(baseColor, Math.min(1.55, 1.0 + (model.priority - 1) * 0.035))
                property color textColor: window.contrastColor(itemColor)

                width: ListView.view.width
                height: summaryContent.implicitHeight + 20
                color: itemColor
                radius: 12

                ColumnLayout {
                    id: summaryContent
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    anchors.topMargin: 10
                    anchors.bottomMargin: 10
                    spacing: 4

                    Text {
                        Layout.fillWidth: true
                        text: model.text
                        color: summaryCard.textColor
                        font.pixelSize: 16
                        wrapMode: Text.Wrap
                    }
                    Text {
                        text: "Приоритет: " + model.priority
                        font.pixelSize: 12
                        color: summaryCard.textColor
                        opacity: 0.75
                    }
                }
            }
        }
    }
}
