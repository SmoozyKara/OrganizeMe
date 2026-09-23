#include "tasklistmodel.h"

#include <algorithm>

TaskListModel::TaskListModel(Organizer& organizer, int listId, QObject* parent)
    : QAbstractListModel(parent), organizer_(organizer), list_id_(listId) {}

TaskList* TaskListModel::findList() const {
    auto it = std::find_if(
        organizer_.lists.begin(), organizer_.lists.end(),
        [this](const TaskList& list) { return list.id == list_id_; });
    return it != organizer_.lists.end() ? &(*it) : nullptr;
}

int TaskListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    TaskList* list = findList();
    return list ? static_cast<int>(list->items.size()) : 0;
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const {
    TaskList* list = findList();
    if (!list || !index.isValid() ||
        index.row() >= static_cast<int>(list->items.size())) {
        return QVariant();
    }

    const TaskItem& item = list->items[index.row()];
    switch (role) {
    case ItemIdRole:
        return item.id;
    case PriorityRole:
        return item.priority;
    case TextRole:
        return QString::fromStdString(item.text);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TaskListModel::roleNames() const {
    return {
            {ItemIdRole, "itemId"},
            {PriorityRole, "priority"},
            {TextRole, "text"},
            };
}

void TaskListModel::addItem(int priority, const QString& text) {
    TaskList* list = findList();
    if (!list) {
        return;
    }

    // Простой вариант: говорим QML "перечитай список целиком" вокруг
    // изменения, вместо точного указания строки вставки. Проще и без
    // риска рассинхрона, но без плавной анимации именно вставленной
    // строки - к этому можно будет вернуться отдельно.
    beginResetModel();
    AddElementToList(list->items, {0, priority, text.toStdString()});
    endResetModel();
}

void TaskListModel::removeItem(int itemId) {
    TaskList* list = findList();
    if (!list) {
        return;
    }

    beginResetModel();
    DeleteElementFromList(list->items, itemId);
    endResetModel();
}

void TaskListModel::changePriority(int itemId, int newPriority) {
    TaskList* list = findList();
    if (!list) {
        return;
    }

    beginResetModel();
    ChangeElementPriority(list->items, itemId, newPriority);
    endResetModel();
}