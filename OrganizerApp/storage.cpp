#include "storage.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <utility>

namespace {

QJsonObject TaskItemToJson(const TaskItem& item) {
    QJsonObject obj;
    obj["id"] = item.id;
    obj["priority"] = item.priority;
    obj["text"] = QString::fromStdString(item.text);
    return obj;
}

TaskItem TaskItemFromJson(const QJsonObject& obj) {
    TaskItem item;
    item.id = obj["id"].toInt();
    item.priority = obj["priority"].toInt();
    item.text = obj["text"].toString().toStdString();
    return item;
}

QJsonObject TaskListToJson(const TaskList& list) {
    QJsonObject obj;
    obj["id"] = list.id;
    obj["rank"] = list.rank;
    obj["name"] = QString::fromStdString(list.name);
    obj["color"] = QString::fromStdString(list.color);

    QJsonArray items;
    for (const TaskItem& item : list.items) {
        items.append(TaskItemToJson(item));
    }
    obj["items"] = items;

    return obj;
}

TaskList TaskListFromJson(const QJsonObject& obj) {
    TaskList list;
    list.id = obj["id"].toInt();
    list.rank = obj["rank"].toInt();
    list.name = obj["name"].toString().toStdString();
    list.color = obj["color"].toString().toStdString();

    for (const QJsonValue& value : obj["items"].toArray()) {
        list.items.push_back(TaskItemFromJson(value.toObject()));
    }

    return list;
}

}  // namespace

bool SaveOrganizerToFile(const Organizer& organizer, const QString& filePath) {
    QJsonArray listsArray;
    for (const TaskList& list : organizer.lists) {
        listsArray.append(TaskListToJson(list));
    }

    QJsonObject root;
    root["lists"] = listsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    return true;
}

bool LoadOrganizerFromFile(Organizer& organizer, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return false;
    }

    Organizer loaded;
    int maxItemId = 0;
    int maxListId = 0;

    // Сохраняем doc.object() в переменную вместо вызова
    // doc.object()["lists"] напрямую - operator[] у временного
    // QJsonObject может привести к лишнему копированию внутренних
    // данных (clazy предупреждал об этом как раз про эту строку).
    const QJsonObject root = doc.object();
    for (const QJsonValue& value : root["lists"].toArray()) {
        TaskList list = TaskListFromJson(value.toObject());
        maxListId = std::max(maxListId, list.id);
        for (const TaskItem& item : list.items) {
            maxItemId = std::max(maxItemId, item.id);
        }
        loaded.lists.push_back(std::move(list));
    }

    organizer.lists = std::move(loaded.lists);

    // +1, потому что счётчики хранят "следующий id, который будет
    // выдан", а не "последний использованный".
    EnsureNextIdAtLeast(maxItemId + 1);
    EnsureNextListIdAtLeast(maxListId + 1);

    return true;
}