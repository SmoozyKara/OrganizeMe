#include "appcontroller.h"

#include <QDir>
#include <QStandardPaths>

#include "storage.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    // Папка, куда Qt рекомендует класть данные приложения - разная на
    // каждой платформе (на Mac это что-то вроде
    // ~/Library/Application Support/<имя приложения>), но нам не нужно
    // знать точный путь - Qt сам его определяет.
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);  // создаёт папку, если её ещё нет
    storage_file_path_ = dir + "/organizer.json";

    bool loaded = LoadOrganizerFromFile(organizer_, storage_file_path_);

    if (!loaded) {
        // Файла ещё нет (первый запуск) или он повреждён - создаём
        // тестовые данные, чтобы экран не был пустым.
        CreateTaskList(organizer_, "Работа", "#FF5733");
        AddElementToList(organizer_.lists.back().items, {0, 1, "Утренний дейлик"});
        AddElementToList(organizer_.lists.back().items, {0, 2, "Обед с коллегой"});

        CreateTaskList(organizer_, "Домашние дела", "#33A1FF");
        AddElementToList(organizer_.lists.back().items, {0, 1, "Генеральная уборка"});
    }

    organizer_model_ = new OrganizerModel(organizer_, this);
    summary_model_ = new SummaryModel(organizer_, this);
}

QObject* AppController::summaryModel() const {
    return summary_model_;
}

void AppController::save() {
    SaveOrganizerToFile(organizer_, storage_file_path_);
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