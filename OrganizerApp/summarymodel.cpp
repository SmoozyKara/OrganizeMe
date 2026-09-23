#include "summarymodel.h"

#include <algorithm>

SummaryModel::SummaryModel(Organizer& organizer, QObject* parent)
    : QAbstractListModel(parent), organizer_(organizer) {
    RebuildCache();
}

// Пересобирает cached_items_ и cached_colors_ вместе, чтобы они
// оставались согласованы по размеру и порядку.
void SummaryModel::RebuildCache() {
    cached_items_ = MergeAllLists(organizer_.lists);

    cached_colors_.clear();
    cached_colors_.reserve(cached_items_.size());

    for (const TaskItem& item : cached_items_) {
        // MergeAllLists не сохраняет, из какого списка пришёл элемент -
        // ищем список, содержащий элемент с таким id, чтобы взять его
        // цвет. При наших масштабах (сотни элементов, десятки списков)
        // такой поиск не создаёт заметной нагрузки.
        std::string color;
        for (const TaskList& list : organizer_.lists) {
            auto it = std::find_if(
                list.items.begin(), list.items.end(),
                [&item](const TaskItem& candidate) { return candidate.id == item.id; });
            if (it != list.items.end()) {
                color = list.color;
                break;
            }
        }
        cached_colors_.push_back(color);
    }
}

int SummaryModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(cached_items_.size());
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() ||
        index.row() >= static_cast<int>(cached_items_.size())) {
        return QVariant();
    }

    const TaskItem& item = cached_items_[index.row()];
    switch (role) {
    case ItemIdRole:
        return item.id;
    case PriorityRole:
        return item.priority;
    case TextRole:
        return QString::fromStdString(item.text);
    case ListColorRole:
        return QString::fromStdString(cached_colors_[index.row()]);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> SummaryModel::roleNames() const {
    return {
            {ItemIdRole, "itemId"},
            {PriorityRole, "priority"},
            {TextRole, "text"},
            {ListColorRole, "listColor"},
            };
}

void SummaryModel::refresh() {
    beginResetModel();
    RebuildCache();
    endResetModel();
}