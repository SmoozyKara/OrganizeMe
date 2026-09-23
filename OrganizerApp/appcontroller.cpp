#include "appcontroller.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    // Тестовые данные - временно, пока нет загрузки из файла.
    CreateTaskList(organizer_, "Работа", "#FF5733");
    int workListId = organizer_.lists.back().id;
    AddElementToList(organizer_.lists.back().items, {0, 1, "Утренний дейлик"});
    AddElementToList(organizer_.lists.back().items, {0, 2, "Обед с коллегой"});

    // "this" - AppController становится родителем модели, значит Qt
    // сам удалит task_list_model_ при уничтожении AppController.
    task_list_model_ = new TaskListModel(organizer_, workListId, this);
}

QObject* AppController::taskListModel() const {
    return task_list_model_;
}