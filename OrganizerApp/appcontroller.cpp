#include "appcontroller.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    // Тестовые данные - временно, пока нет загрузки из файла.
    CreateTaskList(organizer_, "Работа", "#FF5733");
    AddElementToList(organizer_.lists.back().items, {0, 1, "Утренний дейлик"});
    AddElementToList(organizer_.lists.back().items, {0, 2, "Обед с коллегой"});

    CreateTaskList(organizer_, "Домашние дела", "#33A1FF");
    AddElementToList(organizer_.lists.back().items, {0, 1, "Генеральная уборка"});

    organizer_model_ = new OrganizerModel(organizer_, this);
}

QObject* AppController::organizerModel() const {
    return organizer_model_;
}

QObject* AppController::getTaskListModel(int listId) {
    auto it = task_list_models_.find(listId);
    if (it != task_list_models_.end()) {
        return it->second;
    }

    TaskListModel* model = new TaskListModel(organizer_, listId, this);
    task_list_models_[listId] = model;
    return model;
}