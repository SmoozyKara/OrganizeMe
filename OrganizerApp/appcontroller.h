#pragma once

#include <QObject>

#include "task_list.h"
#include "tasklistmodel.h"

// Единая точка, которую main.cpp создаёт и передаёт в QML.
// Владеет всеми данными приложения (Organizer) и всеми QML-моделями
// поверх этих данных. main.cpp благодаря этому классу остаётся
// маленьким - никакой бизнес-логики внутри него самого.
class AppController : public QObject {
    Q_OBJECT

    // Q_PROPERTY делает taskListModel видимым и читаемым из QML - как
    // будто это "поле" объекта appController. READ taskListModel
    // означает "чтобы прочитать это свойство, вызови метод
    // taskListModel()". CONSTANT говорит Qt, что значение не меняется
    // после создания (сама модель не меняется, хотя данные внутри неё -
    // меняются; так что уведомлять QML об изменении самого свойства не
    // нужно).
    Q_PROPERTY(QObject* taskListModel READ taskListModel CONSTANT)

public:
    explicit AppController(QObject* parent = nullptr);

    QObject* taskListModel() const;

private:
    // Данные приложения - пока с тестовым содержимым, позже здесь
    // появится загрузка из файла.
    Organizer organizer_;

    // "this" вторым аргументом ниже (в .cpp) делает AppController
    // родителем этой модели - при уничтожении AppController модель
    // будет удалена автоматически.
    TaskListModel* task_list_model_;
};